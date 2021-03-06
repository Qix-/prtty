#ifndef PRTTY_H
#define PRTTY_H
#pragma once

/*
	Please note that this library does NOT support
	delays (millisecond delays). Please, stop using
	outdated technology and let's move on.

	This library assumes compiled files are the
	ncurses extended compiled formats. If they're not,
	please stop using outdated technology and let's
	move on.
*/

#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <string>
#include <stack>
#include <stdexcept>
#include <sstream>
#include <vector>

namespace prtty {
	using namespace std;

	struct term;
	term get(string termname, string basePath);

	class PrttyError : public runtime_error {
	public:
		template <typename... Args>
		PrttyError(Args... args)
				: runtime_error(args...) {
		}
	};

	namespace impl {

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

			string toString() const {
				switch (this->type) {
				case Type::STRING: return "\"" + string(this->tstring) + "\"";
				case Type::INT: return to_string(this->tint);
				case Type::CHAR: return "'" + string(1, this->tchar) + "'";
				}

#				ifndef __clang__
				// mainly to make GCC happy.
				throw prtty::PrttyError("invalid type; cannot convert to string");
#				endif
			}
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
			typedef vector<shared_ptr<Operation>>::const_iterator OpItr;

			virtual ~Operation() = default;

			virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) = 0;

			virtual bool isControlOp() const {
				return false;
			}

			virtual bool isControlTerminal() const {
				return false;
			}
		};

		namespace op {
			struct StringLiteral : public Operation {
				const string literal;

				StringLiteral(string literal) : literal(literal) {}
				virtual ~StringLiteral() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) data;
					stream << this->literal;
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) stream;
					data.stk.push(data.params[this->arg]);
				}
			};

			struct PushStrlen : public Operation {
				virtual ~PushStrlen() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) stream;
					int len = 0;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						len = static_cast<int>(to_string(v.tint).length());
						break;
					case Any::Type::CHAR:
						len = 1;
						break;
					case Any::Type::STRING:
						len = static_cast<int>(::strlen(v.tstring));
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) stream;
					data.stk.push(this->literal);
				}
			};

			struct PushLiteralChar : public Operation {
				const char literal;

				PushLiteralChar(char literal) : literal(literal) {}
				virtual ~PushLiteralChar() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) stream;
					data.stk.push(data.sparm[this->arg]);
				}
			};

			struct PopWriteString : public Operation {
				bool left;
				int width;
				int precision;

				PopWriteString(bool left=false, int width=-1, int precision=-1)
						: left(left)
						, width(width)
						, precision(precision) {
				}

				virtual ~PopWriteString() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;

					Any &v = data.stk.top();
					string result;

					switch (v.type) {
					case Any::Type::INT:
						result = to_string(v.tint);
						break;
					case Any::Type::CHAR:
						result = string(1, v.tchar);
						break;
					case Any::Type::STRING:
						result = string(v.tstring);
						break;
					}
					data.stk.pop();

					if (this->precision > -1) {
						result = result.substr(0, static_cast<size_t>(this->precision));
					}

					ios::fmtflags f(stream.flags());

					if (this->width > -1) {
						stream << std::setw(this->width);
					}
					if (this->left) {
						stream << std::left;
					}

					stream << result;
					stream.flags(f);
				}
			};

			struct PopWriteInt : public Operation {
				bool left;
				int width;
				bool sign;

				PopWriteInt(bool left=false, int width=-1, bool sign=false)
						: left(left)
						, width(width)
						, sign(sign) {
				}

				virtual ~PopWriteInt() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;

					Any &v = data.stk.top();

					ios::fmtflags f(stream.flags());

					if (this->width > -1) {
						stream << std::setw(this->width);
					}
					if (this->left) {
						stream << std::left;
					}
					if (this->sign) {
						stream << std::showpos;
					}

					switch (v.type) {
					case Any::Type::INT:
						stream << v.tint;
						break;
					case Any::Type::CHAR:
						stream << static_cast<int>(v.tchar);
						break;
					case Any::Type::STRING:
						stream << 0;
						break;
					}
					data.stk.pop();

					stream.flags(f);
				}
			};

			struct PopWriteOct : public PopWriteInt {
				bool exmode;

				PopWriteOct(bool left=false, int width=-1, bool sign=false, bool exmode=false)
						: PopWriteInt(left, width, sign)
						, exmode(exmode) {
				}

				virtual ~PopWriteOct() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;

					ios::fmtflags f(stream.flags());
					stream << oct;
					if (this->exmode) {
						stream << std::showbase;
					}
					this->PopWriteInt::operator()(stream, data, citr, cend);
					stream.flags(f);
				}
			};

			struct PopWriteHex : public PopWriteInt {
				bool exmode;

				PopWriteHex(bool left=false, int width=-1, bool sign=false, bool exmode=false)
						: PopWriteInt(left, width, sign)
						, exmode(exmode) {
				}

				virtual ~PopWriteHex() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;

					ios::fmtflags f(stream.flags());
					stream << hex;
					if (this->exmode) {
						stream << std::showbase;
					}
					this->PopWriteInt::operator()(stream, data, citr, cend);
					stream.flags(f);
				}
			};

			struct PopWriteUHex : public PopWriteHex {
				PopWriteUHex(bool left=false, int width=-1, bool sign=false, bool exmode=false)
						: PopWriteHex(left, width, sign, exmode) {
				}

				virtual ~PopWriteUHex() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;

					ios::fmtflags f(stream.flags());
					stream << uppercase;
					this->PopWriteHex::operator()(stream, data, citr, cend);
					stream.flags(f);
				}
			};

			struct PopWriteChar : public Operation {
				virtual ~PopWriteChar() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					Any &v = data.stk.top();
					switch (v.type) {
					case Any::Type::INT:
						stream << string(1, static_cast<char>(v.tint & 0xFF));
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

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) citr;
					(void) cend;
					(void) stream;
					if (data.params[0].type == Any::Type::INT) data.params[0].tint++;
					if (data.params[1].type == Any::Type::INT) data.params[1].tint++;
				}
			};

			struct Cond : public Operation {
				virtual ~Cond() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;

					++citr;
					for (; citr != cend && !(*citr)->isControlOp(); citr++) {
						(**citr)(stream, data, citr, cend);
					}
				}

				virtual bool isControlOp() const {
					return true;
				}
			};

			struct CondThen : public Cond {
				virtual ~CondThen() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;

					bool enable = false;
					Any &val = data.stk.top();
					switch (val.type) {
					case Any::Type::INT:
						enable = val.tint != 0;
						break;
					case Any::Type::CHAR:
						enable = val.tchar != '\0';
						break;
					case Any::Type::STRING:
						enable = val.tstring[0] != '\0';
						break;
					}
					data.stk.pop();

					if (enable) {
						this->Cond::operator()(stream, data, citr, cend);
					} else {
						while (citr != cend && !(*citr)->isControlTerminal()) {
							++citr;
						}
					}
				}
			};

			struct CondEnd : public Operation {
				virtual ~CondEnd() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;
					(void) citr;
					(void) cend;

					// Can't tell if this is necessary.
					data.stk.pop();
				}

				virtual bool isControlOp() const {
					return true;
				}

				virtual bool isControlTerminal() const {
					return true;
				}
			};

#			define BinOp(name, verb, operand) struct name : public Operation { \
				virtual ~name() = default; \
				\
				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) { \
					(void) stream; \
					(void) citr; \
					(void) cend; \
					\
					Any &rop = data.stk.top(); \
					data.stk.pop(); \
					Any &lop = data.stk.top(); \
					data.stk.pop(); \
					\
					int result = 0; \
					\
					switch (lop.type) { \
					case Any::Type::INT: \
						result = lop.tint; \
						break; \
					case Any::Type::CHAR: \
						result = lop.tchar; \
						break; \
					case Any::Type::STRING: \
					nostrings: \
						throw prtty::PrttyError("cannot " verb " a string operand: " + lop.toString() + " " #operand " " + rop.toString()); \
					} \
					\
					switch (rop.type) { \
					case Any::Type::INT: \
						result = result operand rop.tint; \
						break; \
					case Any::Type::CHAR: \
						result = result operand rop.tchar; \
						break; \
					case Any::Type::STRING: \
						goto nostrings; \
					} \
					\
					data.stk.push(result); \
				} \
			}

			BinOp(ArithAdd, "add", +);
			BinOp(ArithSub, "subtract", -);
			BinOp(ArithMul, "multiply", *);
			BinOp(ArithDiv, "divide", /);
			BinOp(ArithMod, "modulo (find the remainder of)", %);

			BinOp(BitOr, "bitwise-or", |);
			BinOp(BitAnd, "bitwise-and", &);
			BinOp(BitXor, "bitwise-xor", ^);

			BinOp(BoolGt, "perform greater-than conditionals on", >);
			BinOp(BoolLt, "perform less-than conditionals on", <);
			BinOp(BoolAnd, "perform boolean-and conditionals on", &&);
			BinOp(BoolOr, "perform boolean-or conditionals on", ||);

#			undef BinOp

			struct BoolEq : public Operation {
				virtual ~BoolEq() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;
					(void) citr;
					(void) cend;

					Any &rop = data.stk.top();
					data.stk.pop();
					Any &lop = data.stk.top();
					data.stk.pop();

					int result = 0;

					switch (lop.type) {
					case Any::Type::INT:
						result = lop.tint;
						break;
					case Any::Type::CHAR:
						result = lop.tchar;
						break;
					case Any::Type::STRING:
						if (rop.type != Any::Type::STRING) {
							throw prtty::PrttyError("cannot determine equality of mixed string/integral operands: " + lop.toString() + " == " + rop.toString());
						}
					}

					switch (rop.type) {
					case Any::Type::INT:
						result = result == rop.tint;
						break;
					case Any::Type::CHAR:
						result = result == rop.tchar;
						break;
					case Any::Type::STRING:
						result = strcmp(lop.tstring, rop.tstring) == 0;
						break;
					}

					data.stk.push(result);
				}
			};

			struct BoolNot : public Operation {
				virtual ~BoolNot() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;
					(void) citr;
					(void) cend;

					Any &val = data.stk.top();
					data.stk.pop();

					switch (val.type) {
					case Any::Type::INT:
						data.stk.push(val.tint == 0);
						break;
					case Any::Type::CHAR:
						data.stk.push(val.tchar == '\0');
						break;
					case Any::Type::STRING:
						// ehh.. this is kind of an 'extension'
						data.stk.push(val.tstring[0] == '\0');
						break;
					}
				}
			};

			struct BitNegate : public Operation {
				virtual ~BitNegate() = default;

				virtual void operator()(ostream &stream, Data &data, OpItr &citr, OpItr &cend) {
					(void) stream;
					(void) citr;
					(void) cend;

					Any &val = data.stk.top();
					data.stk.pop();

					switch (val.type) {
					case Any::Type::INT:
						data.stk.push(~val.tint);
						break;
					case Any::Type::CHAR:
						data.stk.push(~val.tchar);
						break;
					case Any::Type::STRING:
						throw prtty::PrttyError("cannot bitwise negate a string: ~" + val.toString());
					}
				}
			};
		}

		template <typename T, typename... Args>
		shared_ptr<T> mkunique(Args... args) {
			return shared_ptr<T>(new T(args...));
		}

		struct Sequence {
			Sequence() = default;

			Sequence(const Sequence &other)
					: nargs(other.nargs)
					, ops(other.ops) {
			}

			Sequence & operator =(const Sequence &other) {
				this->nargs = other.nargs;
				this->ops = other.ops;
				return *this;
			}

			template <typename... Args>
			void operator ()(Data &data, ostream &stream, Args... args) const {
				data.session(args...);

				ios::fmtflags f(stream.flags());
				stream << dec;
				Operation::OpItr citr = this->ops.cbegin();
				Operation::OpItr cend = this->ops.cend();
				for (; citr != cend; citr++) {
					(**citr)(stream, data, citr, cend);
				}
				stream.flags(f);
			}

			int nargs;
			vector<shared_ptr<Operation>> ops;

			static Sequence parse(const string fmt) {
				Sequence seq;

				size_t len = fmt.length();
				if (len == 0) {
					return seq;
				}

				seq.nargs = 0;

				unique_ptr<char[]> literal(new char[len]);
				size_t lc = 0;
				char c = 0;
				int arg = 0;

				for (size_t i = 0; i < len; i++) {
					c = fmt[i];

					if (c == '%') {
						c = fmt[++i];

						if (c == '%') {
							goto addLiteral;
						}

						if (lc > 0) {
							seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
							lc = 0;
						}

						if (c >= '0' && c <= '9') {
							goto fieldParse;
						}

						switch (c) {
						case ':':
						case '#':
						case '.':
							goto fieldParse;
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

							arg = c - '0';
							seq.nargs = seq.nargs > arg ? seq.nargs : arg;
							seq.ops.push_back(mkunique<op::PushArg>(static_cast<unsigned int>(arg - 1)));
							break;
						case 'P':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PopSetDynamic>(static_cast<unsigned int>(c - 'a')));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PopSetStatic>(static_cast<unsigned int>(c - 'A')));
							} else {
								throw prtty::PrttyError("set dynamic/static variable escape (%P) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case 'g':
							c = fmt[++i];
							if (c >= 'a' && c<= 'z') {
								seq.ops.push_back(mkunique<op::PushDynamic>(static_cast<unsigned int>(c - 'a')));
							} else if (c >= 'A' && c <= 'Z') {
								seq.ops.push_back(mkunique<op::PushStatic>(static_cast<unsigned int>(c - 'A')));
							} else {
								throw prtty::PrttyError("get dynamic/static variable escape (%g) must be followed by a character within a-Z or A-Z: %P" + string(1, c));
							}
							break;
						case '\'':
							c = fmt[++i];
							if (fmt[++i] != '\'') {
								throw prtty::PrttyError("character literal was unterminated (expected '): %'" + string(1, c) + "'");
							}

							seq.ops.push_back(mkunique<op::PushLiteralChar>(c));
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

							seq.ops.push_back(mkunique<op::PushLiteralInt>(arg * sign));
							break;
						}
						case 'l':
							seq.ops.push_back(mkunique<op::PushStrlen>());
							break;
						case 'i':
							seq.nargs = seq.nargs > 2 ? seq.nargs : 2;
							seq.ops.push_back(mkunique<op::IncrementFirstTwo>());
							break;
						case '?':
						case 'e': // these are handled identically - though I admit the docs don't define these well.
							seq.ops.push_back(mkunique<op::Cond>());
							break;
						case 't':
							seq.ops.push_back(mkunique<op::CondThen>());
							break;
						case ';':
							seq.ops.push_back(mkunique<op::CondEnd>());
							break;
						case '+':
							seq.ops.push_back(mkunique<op::ArithAdd>());
							break;
						case '-':
							seq.ops.push_back(mkunique<op::ArithSub>());
							break;
						case '*':
							seq.ops.push_back(mkunique<op::ArithMul>());
							break;
						case '/':
							seq.ops.push_back(mkunique<op::ArithDiv>());
							break;
						case 'm':
							seq.ops.push_back(mkunique<op::ArithMod>());
							break;
						case '&':
							seq.ops.push_back(mkunique<op::BitAnd>());
							break;
						case '|':
							seq.ops.push_back(mkunique<op::BitOr>());
							break;
						case '^':
							seq.ops.push_back(mkunique<op::BitXor>());
							break;
						case '=':
							seq.ops.push_back(mkunique<op::BoolEq>());
							break;
						case '>':
							seq.ops.push_back(mkunique<op::BoolGt>());
							break;
						case '<':
							seq.ops.push_back(mkunique<op::BoolLt>());
							break;
						case 'A':
							seq.ops.push_back(mkunique<op::BoolAnd>());
							break;
						case 'O':
							seq.ops.push_back(mkunique<op::BoolOr>());
							break;
						case '!':
							seq.ops.push_back(mkunique<op::BoolNot>());
							break;
						case '~':
							seq.ops.push_back(mkunique<op::BitNegate>());
							break;
						default:
#							ifdef __clang__
#								pragma clang diagnostic push
#								pragma clang diagnostic ignored "-Wunreachable-code"
#							endif
							throw prtty::PrttyError("invalid escape: %" + (c == 0 ? string("<EOF>") : string(1, c)));
#							ifdef __clang__
#								pragma clang diagnostic pop
#							endif
						}

						goto afterFieldParse;
					fieldParse:
						{
							bool exmode = false;
							bool sign = false;
							bool left = false;
							int width = -1;
							bool usePrecision = false;
							int precision = -1;

							// parses extended flag notation for field width/precision
							for (; i < len; i++) {
								c = fmt[i];

								if (c >= '0' && c <= '9') {
									int *target = usePrecision ? &precision : &width;
									*target = 0;

									// this is some gross logic. might be nice to figure out
									// a better way to do this.
									for (; i < len && (c = fmt[i]) >= '0' && c <= '9'; i++) {
										c = fmt[i];
										*target *= 10;
										*target += c - '0';

										if ((c = fmt[i + 1]) < '0' || c > '9') {
											break;
										}
									}

									continue;
								}

								switch (c) {
								case ':': continue;
								case '-': left = true; break;
								case '+': sign = true; break;
								case '#': exmode = true; break;
								case '.': usePrecision = true; break;
								case 's':
									seq.ops.push_back(mkunique<op::PopWriteString>(left, width, precision));
									goto afterFieldParse;
								case 'd':
									seq.ops.push_back(mkunique<op::PopWriteInt>(left, width, sign));
									goto afterFieldParse;
								case 'x':
									seq.ops.push_back(mkunique<op::PopWriteHex>(left, width, sign, exmode));
									goto afterFieldParse;
								case 'X':
									seq.ops.push_back(mkunique<op::PopWriteUHex>(left, width, sign, exmode));
									goto afterFieldParse;
								case 'o':
									seq.ops.push_back(mkunique<op::PopWriteOct>(left, width, sign, exmode));
									goto afterFieldParse;
								}
							}
						}

					afterFieldParse: (void)0;
					} else {
					addLiteral:
						literal[lc++] = c;
					}
				}

				if (lc > 0) {
					seq.ops.push_back(mkunique<op::StringLiteral>(string(literal.get(), lc)));
				}

				return seq;
			}
		};

		inline char hashCharacter(char c) {
			if (c < 10) {
				return c + '0';
			} else {
				return (c - 10) + 'a';
			}
		}

		inline void toLE(uint16_t v) {
			volatile uint32_t i = 0x01234567;
			if ((*(reinterpret_cast<volatile uint8_t*>(&i))) == 0x01) {
				v = static_cast<uint16_t>(((v >> 8) & 0xFF) | ((v & 0xFF) << 8));
			}
		}

		inline void split(const string &s, char delim, vector<string> &elems) {
			stringstream ss(s);
			string item;
			while(getline(ss, item, delim)) {
				elems.push_back(item);
			}
		}

		class SequenceStreamer {
			friend prtty::term prtty::get(string termname, string basePath);

			class SeqStreamDeferredCall {
				friend SequenceStreamer;
			public:
				SeqStreamDeferredCall(const SeqStreamDeferredCall &other) = default;

				operator std::string() const noexcept(true) {
					std::stringstream ss;
					ss << *this;
					return ss.str();
				}

			private:
				SeqStreamDeferredCall(function<ostream&(ostream&)> callback)
						: callback(callback) {
				}

				friend ostream & operator <<(ostream &stream, const SeqStreamDeferredCall &defcall) {
					return defcall.callback(stream);
				}

				function<ostream&(ostream&)> callback;
			};

		public:
			SequenceStreamer(Data &data)
					: isSet(false)
					, data(data) {
			}

			explicit operator bool() const noexcept(true) {
				return this->isSet;
			}

			operator std::string() const noexcept(true) {
				std::stringstream ss;
				ss << *this;
				return ss.str();
			}

			bool operator !() const noexcept(true) {
				return !this->isSet;
			}

			template <typename... Args>
			SeqStreamDeferredCall operator()(Args... args) const {
				return function<ostream&(ostream&)>([&, args...](ostream &stream) -> ostream & {
					this->seq(this->data, stream, args...);
					return stream;
				});
			}

		private:
			friend ostream & operator <<(ostream &stream, const SequenceStreamer &seqstream) {
				seqstream.seq(seqstream.data, stream);
				return stream;
			}

			void operator =(const string &seqstr) {
				if (seqstr.empty()) {
					return;
				}

				this->seq = Sequence::parse(seqstr);
				this->isSet = true;
			}

			bool isSet;
			Data &data;
			Sequence seq;
		};
	}

	struct term {
		const string id;
		const vector<string> names;

#		define PRTTY_DO_BOOLEAN(name) const bool name;
#		include "./prtty-booleans.inc"

#		define PRTTY_DO_INTEGER(name) const int name;
#		include "./prtty-integers.inc"

#		define PRTTY_DO_STRING(name) const impl::SequenceStreamer name;
#		include "./prtty-strings.inc"

		term(string id, vector<string> &names)
				: id(id)
				, names(names)
#		define PRTTY_DO_BOOLEAN(name) , name(false)
#		include "./prtty-booleans.inc"
#		define PRTTY_DO_INTEGER(name) , name(0)
#		include "./prtty-integers.inc"
#		define PRTTY_DO_STRING(name) , name(this->data)
#		include "./prtty-strings.inc"
		{}

	private:
		impl::Data data;
	};

	term get(string termname, string basePath)
#	ifdef PRTTY_MAIN
	{
		ifstream dbf;
		string dbPath = basePath + "/" + string(1, termname[0]) + "/" + termname;
		dbf.open(dbPath, ios::binary);
		if (!dbf) {
			char hash[2];
			char firstchar = termname[0];
			hash[0] = impl::hashCharacter((firstchar & 0xF0) >> 4);
			hash[1] = impl::hashCharacter(firstchar & 0x0F);

			dbPath = basePath + "/" + string(&hash[0], 2) + "/" + termname;
			dbf.open(dbPath, ios::binary);

			if (!dbf) {
				throw PrttyError("could not load database for terminal: " + termname + " (from base search path: " + basePath + ")");
			}
		}

		uint8_t _u8;
		uint16_t _u16;

#		define READ_U8() dbf.read(reinterpret_cast<char *>(&_u8), 1)
#		define READ_U16() dbf.read(reinterpret_cast<char *>(&_u16), 2); impl::toLE(_u16)

		// magic number
		READ_U16();
		if (_u16 != 0x11A) {
			throw PrttyError("terminal description file has invalid magic number");
		}

		READ_U16();
		size_t nameSize = _u16;
		READ_U16();
		size_t boolSize = _u16;
		READ_U16();
		size_t numCount = _u16;
		READ_U16();
		size_t offCount = _u16;
		dbf.ignore(2); // string (offset) count

		vector<string> names;

		unique_ptr<char[]> fullNameStr(new char[nameSize]);
		dbf.read(fullNameStr.get(), static_cast<streamsize>(nameSize));
		impl::split(string(fullNameStr.get(), nameSize - 1), '|', names);

		term result(termname, names);

		bool *bools = const_cast<bool *>(&(result.PRTTY_FIRST_BOOLEAN));
#		undef PRTTY_FIRST_BOOLEAN
		for (size_t i = 0; i < boolSize; i++) {
			READ_U8();
			bools[i] = static_cast<bool>(_u8);
		}

		// advance
		if (boolSize > PRTTY_NUM_BOOLEANS) {
			dbf.ignore(static_cast<streamsize>(boolSize - PRTTY_NUM_BOOLEANS));
		}
#		undef PRTTY_NUM_BOOLEANS

		int *ints = const_cast<int *>(&(result.PRTTY_FIRST_INTEGER));
#		undef PRTTY_FIRST_INTEGER
		for (size_t i = 0; i < numCount; i++) {
			READ_U16();
			ints[i] = static_cast<int>(_u16);
		}

		// advance
		if (numCount > PRTTY_NUM_INTEGERS) {
			dbf.ignore(static_cast<streamsize>((numCount - PRTTY_NUM_INTEGERS) * 2));
		}
#		undef PRTTY_NUM_INTEGERS

		char buf[4096];
		size_t offset = offCount * 2;
		auto loadString = [&]() -> string {
			READ_U16();
			offset -= 2;
			if (_u16 == static_cast<uint16_t>(-1)) {
				return "";
			}
			auto curpos = dbf.tellg();
			dbf.ignore(static_cast<streamsize>(offset + _u16));
			dbf.getline(&buf[0], 4096, '\0');
			dbf.seekg(curpos);
			return string(&buf[0]);
		};

		impl::SequenceStreamer *strings = const_cast<impl::SequenceStreamer *>(&(result.PRTTY_FIRST_STRING));
		for (size_t i = 0; i < PRTTY_NUM_STRINGS; i++) {
			strings[i] = loadString();
		}
#		undef PRTTY_FIRST_STRING
#		undef PRTTY_NUM_STRINGS

#		undef READ_U8
#		undef READ_U16
		return result;
	}
#	else
	;
#	endif

	term get(string termname)
#	ifdef PRTTY_MAIN
	{
		char *termdb = getenv("TERMINFO");
		return get(termname, termdb ? termdb : "/usr/share/terminfo");
	}
#	else
	;
#	endif

	term get()
#	ifdef PRTTY_MAIN
	{
		char *termname = getenv("TERM");
		return get(termname ? termname : "dumb");
	}
#	else
	;
#	endif

}

#endif
