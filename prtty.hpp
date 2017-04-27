#ifndef PRTTY_H__
#define PRTTY_H__
#pragma once

#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <stack>
#include <stdexcept>
#include <vector>

namespace prtty {
	class PrttyError : public std::runtime_error {
	public:
		template <typename... Args>
		PrttyError(Args... args)
				: std::runtime_error(args...) {
		}
	};

	namespace impl {
		using namespace std;

		struct Any {
			enum class Type {
				STRING,
				INT,
				CHAR
			};

			Any()
					: type(Type::INT)
					, tint(0) {
			}

			Any(const char *str)
					: type(Type::STRING)
					, tstring(str) {
			}

			Any(int i)
					: type(Type::INT)
					, tint(i) {
			}

			Any(char c)
					: type(Type::CHAR)
					, tchar(c) {
			}

			Type type;

			union {
				const char *tstring;
				int tint;
				char tchar;
			};
		};

		struct Data {
			/*
				we create data structs to avoid re-allocation
				of stacks on each parse. these are hidden from
				the user and stored in `term` objects.
			*/

			Data() {
				this->dparm.reserve(26);
				this->sparm.reserve(26);
				fill(this->dparm.begin(), this->dparm.end(), Any(0));
				fill(this->sparm.begin(), this->sparm.end(), Any(0));
			}

			template <typename... Args>
			void session(Args... args) {
				stack<Any>().swap(this->stk);
				this->dparm.clear();
				this->params = vector<Any>(9, Any());
				this->params.insert(this->params.begin(), {args...});
			}

			stack<Any> stk;
			vector<Any> params;
			vector<Any> dparm;

			vector<Any> sparm;
		};

		struct Operation {
			virtual ~Operation() = default;
			virtual void operator()(ostream &stream, Data &data) = 0;
		};

		namespace op {
			struct StringLiteral : public Operation {
				const string literal;

				StringLiteral(string literal) : literal(literal) {}
				virtual ~StringLiteral() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) data;
					stream << this->literal;
				}
			};

			struct IntLiteral : public Operation {
				const int literal;

				IntLiteral(int literal) : literal(literal) {}
				virtual ~IntLiteral() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) data;
					stream << this->literal;
				}
			};

			struct CharLiteral : public Operation {
				const char literal;

				CharLiteral(char literal) : literal(literal) {}
				virtual ~CharLiteral() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) data;
					stream << string(1, this->literal);
				}
			};

			struct PushArg : public Operation {
				const unsigned int arg;

				PushArg(unsigned int arg) : arg(arg) {
					if (arg > 8) {
						throw prtty::PrttyError("arg value must be between 0-8 (inclusive)");
					}
				}
				virtual ~PushArg() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.stk.push(data.params[this->arg]);
				}
			};

			struct PushStrlen : public Operation {
				virtual ~PushStrlen() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					int len = 0;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						len = to_string(v.tint).length();
						break;
					case Any::Type::CHAR:
						len = 1;
						break;
					case Any::Type::STRING:
						len = ::strlen(v.tstring);
						break;
					}
					data.stk.pop();
					data.stk.push(len);
				}
			};

			struct PushLiteralInt : public Operation {
				const int literal;

				PushLiteralInt(int literal) : literal(literal) {}
				virtual ~PushLiteralInt() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.stk.push(this->literal);
				}
			};

			struct PopSetDynamic : public Operation {
				const unsigned int arg;

				PopSetDynamic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PopSetDynamic() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.dparm[this->arg] = data.stk.top();
					data.stk.pop();
				}
			};

			struct PopSetStatic : public Operation {
				const unsigned int arg;

				PopSetStatic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PopSetStatic() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.sparm[this->arg] = data.stk.top();
					data.stk.pop();
				}
			};

			struct PushDynamic : public Operation {
				const unsigned int arg;

				PushDynamic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PushDynamic() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.stk.push(data.dparm[this->arg]);
				}
			};

			struct PushStatic : public Operation {
				const unsigned int arg;

				PushStatic(unsigned int arg) : arg(arg) {
					if (arg > 25) {
						throw prtty::PrttyError("arg value must be between 0-25 (inclusive)");
					}
				}
				virtual ~PushStatic() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					data.stk.push(data.sparm[this->arg]);
				}
			};

			struct PopWriteString : public Operation {
				virtual ~PopWriteString() = default;

				virtual void operator()(ostream &stream, Data &data) {
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << to_string(v.tint);
						break;
					case Any::Type::CHAR:
						stream << string(1, v.tchar);
						break;
					case Any::Type::STRING:
						stream << string(v.tstring);
						break;
					}
					data.stk.pop();
				}
			};

			struct PopWriteInt : public Operation {
				virtual ~PopWriteInt() = default;

				virtual void operator()(ostream &stream, Data &data) {
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << to_string(v.tint);
						break;
					case Any::Type::CHAR:
						stream << to_string((int) v.tchar);
						break;
					case Any::Type::STRING:
						stream << to_string(0);
						break;
					}
					data.stk.pop();
				}
			};

			struct PopWriteOct : public PopWriteInt {
				virtual ~PopWriteOct() = default;

				virtual void operator()(ostream &stream, Data &data) {
					stream << std::oct;
					this->PopWriteInt::operator()(stream, data);
					stream << std::dec;
				}
			};

			struct PopWriteHex : public PopWriteInt {
				virtual ~PopWriteHex() = default;

				virtual void operator()(ostream &stream, Data &data) {
					stream << std::hex;
					this->PopWriteInt::operator()(stream, data);
					stream << std::dec;
				}
			};

			struct PopWriteUHex : public PopWriteHex {
				virtual ~PopWriteUHex() = default;

				virtual void operator()(ostream &stream, Data &data) {
					stream << std::uppercase;
					this->PopWriteHex::operator()(stream, data);
					stream << std::nouppercase;
				}
			};

			struct PopWriteChar : public Operation {
				virtual ~PopWriteChar() = default;

				virtual void operator()(ostream &stream, Data &data) {
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << string(1, (char) (v.tint & 0xFF));
						break;
					case Any::Type::CHAR:
						stream << string(1, v.tchar);
						break;
					case Any::Type::STRING:
						stream << string(1, v.tstring[0]);
						break;
					}
					data.stk.pop();
				}
			};

			struct IncrementFirstTwo : public Operation {
				virtual ~IncrementFirstTwo() = default;

				virtual void operator()(ostream &stream, Data &data) {
					(void) stream;
					if (data.params[0].type == Any::Type::INT) data.params[0].tint++;
					if (data.params[1].type == Any::Type::INT) data.params[1].tint++;
				}
			};
		}

		template <typename T, typename... Args>
		unique_ptr<T> mkunique(Args... args) {
			return unique_ptr<T>(new T(args...));
		}

		struct Sequence {
			template <typename... Args>
			void operator ()(Data &data, ostream &stream, Args... args) {
				data.session(args...);

				ios::fmtflags f(stream.flags());
				stream << std::dec;
				for (const auto &op : this->ops) {
					(*op)(stream, data);
				}
				stream.flags(f);
			}

			int nargs;
			vector<unique_ptr<Operation>> ops;

			static Sequence parse(const string fmt) {
				Sequence seq;
				seq.nargs = 0;
				size_t len = fmt.length();
				unique_ptr<char[]> literal(new char[len]);
				char lc = 0;
				char c = 0;
				int arg = 0;
				for (size_t i = 0; i < len; i++) {
					c = fmt[i];

					if (c == '%') {
						c = fmt[++i];

						if (c == '%') {
							goto addLiteral;
						}

						if (lc) {
							seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
							lc = 0;
						}

						switch (c) {
						case 'c':
							seq.ops.push_back(mkunique<op::PopWriteChar>());
							break;
						case 's':
							seq.ops.push_back(mkunique<op::PopWriteString>());
							break;
						case 'd':
							seq.ops.push_back(mkunique<op::PopWriteInt>());
							break;
						case 'x':
							seq.ops.push_back(mkunique<op::PopWriteHex>());
							break;
						case 'X':
							seq.ops.push_back(mkunique<op::PopWriteUHex>());
							break;
						case 'o':
							seq.ops.push_back(mkunique<op::PopWriteOct>());
							break;
						case 'p':
							c = fmt[++i];
							if (c < '1' || c > '9') {
								throw prtty::PrttyError("push argument escape (%p) must be followed by a number between 1-9 (inclusive): %p" + string(1, c));
							}

							arg = c - '1';
							seq.nargs = seq.nargs > arg ? seq.nargs : arg;
							seq.ops.push_back(mkunique<op::PushArg>(arg));
							break;
						case 'P':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PopSetDynamic>(c - 'a'));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PopSetStatic>(c - 'A'));
							} else {
								throw prtty::PrttyError("set dynamic/static variable escape (%P) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case 'g':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PushDynamic>(c - 'a'));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PushStatic>(c - 'A'));
							} else {
								throw prtty::PrttyError("get dynamic/static variable escape (%g) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case '\'':
							c = fmt[++i];
							if (fmt[++i] != '\'') {
								throw prtty::PrttyError("character literal was unterminated (expected '): %'" + string(1, c) + "'");
							}

							seq.ops.push_back(mkunique<op::CharLiteral>(c));
							break;
						case '{': {
							++i;
							if (fmt[i] == '}') break;

							arg = 0;

							int sign = 1;
							if (fmt[i] == '-') {
								sign = -1;
							}

							for (; i < len && fmt[i] != '}'; i++) {
								c = fmt[i];
								if (c < '0' || c > '9') {
									throw prtty::PrttyError("found invalid number literal");
								}
								arg *= 10;
								arg += fmt[i] - '0';
							}

							seq.ops.push_back(mkunique<op::IntLiteral>(arg * sign));
							break;
						}
						case 'l':
							seq.ops.push_back(mkunique<op::PushStrlen>());
							break;
						case 'i':
							seq.nargs = seq.nargs > 2 ? seq.nargs : 2;
							seq.ops.push_back(mkunique<op::IncrementFirstTwo>());
							break;
						default:
							throw prtty::PrttyError("invalid escape: %" + (c == 0 ? string("<EOF>") : string(1, c)));
						}
					} else {
					addLiteral:
						literal[lc++] = c;
					}
				}

				if (lc) {
					seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
				}

				return seq;
			}
		};
	}
}

#endif
