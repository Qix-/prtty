#ifndef PRTTY_H__
#define PRTTY_H__
#pragma once

/*
	This implementation does not support static tparms (%g)
	and will just create another dynamic set of them.

	This is both for practical reasons (avoiding unions,
	memory copying, lifetimes, etc.) as well as
	philosophical reasons (terminfo is too archaic and
	overcomplicated).
*/

#include <functional>
#include <ostream>
#include <string>
#include <stack>
#include <stdexcept>
#include <tuple>
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

	struct Data {
		/*
			we create data structs to avoid re-allocation
			of stacks on each parse. these are hidden from
			the user and stored in `term` objects.
		*/

		Data() {
			this->dparm.reserve(26);
			this->sparm.reserve(26);
			fill(this->dparm.begin(), this->dparm.end(), 0);
			fill(this->sparm.begin(), this->sparm.end(), 0);
		}

		stack<int> stack;
		vector<int> dparm;
		vector<int> sparm;
	};

	template<long long index, typename... Ts>
	constexpr bool isValidIndex() {
		return index >= 0 && index < sizeof...(Ts);
	}

	template<size_t index, typename T, typename... Ts>
	inline constexpr typename enable_if<index==0, T>::type
	getArg(T&& t, Ts&&... ts) {
#		pragma unused(ts)
		return t;
	}

	template<size_t index, typename T, typename... Ts>
	inline constexpr typename enable_if<(index > 0) && index <= sizeof...(Ts), typename tuple_element<index, tuple<T, Ts...>>::type>::type
	getArg(T&& t, Ts&&... ts) {
#		pragma unused(t)
		return getArg<index-1>(forward<Ts>(ts)...);
	}

	namespace render {
		template <typename T>
		string toString(T v) {
			return to_string(v);
		}

		template<>
		string toString(const char *v) {
			return string(v);
		}

		template<>
		string toString(char v) {
			return string(1, v);
		}

		template <int param, typename... Args>
		struct string {
			inline static void invoke(ostream &stream, const Data &data, Args... args) {
				(void) data; // TODO use data to determine field width, etc.
				stream << toString(getArg<param, Args...>(args...)) << endl;
			}
		};

		template <template <int, typename... FArgs> class Fn, typename... FArgs>
		void invoke(int arg, ostream &stream, const Data &data, FArgs... args) {
			switch (arg) {
			case 0: return Fn<0, FArgs...>::invoke(stream, data, args...);
			case 1: return Fn<1, FArgs...>::invoke(stream, data, args...);
			case 2: return Fn<2, FArgs...>::invoke(stream, data, args...);
			case 3: return Fn<3, FArgs...>::invoke(stream, data, args...);
			case 4: return Fn<4, FArgs...>::invoke(stream, data, args...);
			case 5: return Fn<5, FArgs...>::invoke(stream, data, args...);
			case 6: return Fn<6, FArgs...>::invoke(stream, data, args...);
			case 7: return Fn<7, FArgs...>::invoke(stream, data, args...);
			case 8: return Fn<8, FArgs...>::invoke(stream, data, args...);
			}
		}
	}

	template <typename... Args>
	void exec(Data &data, ostream &stream, const string &fmt, Args... args) {
		size_t len = fmt.length();

		unsigned int arg;
		char c;

		for (size_t i = 0; i < len; i++) {
			c = fmt[i];

			if (c == '%') {
				c = fmt[++i];
				switch (c) {
				case '%': // literal '%'
					stream << '%';
					break;
				case 'p': // push the nth argument
					c = fmt[++i];
					arg = c - '1';
					if (arg > 8) {
						throw prtty::PrttyError("invalid push: must specify 1-9: " + string(1, c));
					}

					if (arg >= sizeof...(args)) {
						throw prtty::PrttyError("not enough arguments: escape asked for argument " + string(1, c));
					}

					data.stack.push(arg);
					break;
				case 'P': // pop() -> variable
					c = fmt[++i];
					if (c >= 'a' && c <= 'z') {
						// dynamic
						arg = c - 'a';
						data.dparm[arg] = data.stack.top();
					} else if (c >= 'A' && c <= 'Z') {
						// "static"
						arg = c - 'A';
						data.sparm[arg] = data.stack.top();
					} else {
						throw prtty::PrttyError("invalid variable set escape: must be a-z or A-Z: " + string(1, c));
					}
					data.stack.pop();
					break;
				case 'g': // variable -> push()
					c = fmt[++i];
					if (c >= 'a' && c <= 'z') {
						// dynamic
						arg = c - 'a';
						data.stack.push(data.dparm[arg]);
					} else if (c >= 'A' && c <= 'Z') {
						// "static"
						arg = c - 'A';
						data.stack.push(data.sparm[arg]);
					} else {
						throw prtty::PrttyError("invalid variable get escape: must be a-z or A-Z: " + string(1, c));
					}
					break;
				case 's': // display string
					arg = data.stack.top();
					data.stack.pop();
					render::invoke<render::string>(arg, stream, data, args...);
					break;
				default:
					throw prtty::PrttyError("invalid escape sequence: " + (c == 0 ? string("\\0") : string(1, c)));
				}
			} else {
				stream << c;
			}
		}

		stack<int>().swap(data.stack);
		data.dparm.clear();
		data.sparm.clear();
	}
}

}

#endif
