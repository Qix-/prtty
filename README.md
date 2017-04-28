# prTTY
Simplified Terminfo parser and a neat little wrapper/implementation of the terminfo "scripting" language.

| **Travis**   | [![Travis](https://img.shields.io/travis/qix-/prtty.svg?style=flat-square)](https://travis-ci.org/qix-/prtty/builds)               |
|--------------|----------------------------------------------------------------------------------------------------------------------------------------|
| **CircleCI** | [![CircleCI](https://img.shields.io/circleci/project/github/qix-/prtty.svg?style=flat-square)](https://circleci.com/gh/qix-/prtty) |

Aims to have a similar API as [zachariahreed/pyterminfo](https://github.com/zachariahreed/pyterminfo).
There are several of these out there but they all take shortcuts or don't help you actually _use_ the
values.

## Usage
prTTY provides a `term` object that holds all of the boolean, numeric, and string&ast; "capabilities"
for the given terminal as properties using their long names.

A list of capabilities with their descriptions and expected arguments can be found
[in this Gist](https://gist.github.com/rwboyer/1691527#file-openbsd-terminfo-L106-L588).

You can also find it in most distributions' man page for `terminfo(5)`.

```c++
#include <iostream>

#define PRTTY_MAIN // define this in exactly one translation unit
#include "prtty.hpp"

using namespace std;

int main() {
	// get the current terminal (using $TERM) or maually specify the name.
	prtty::term term = prtty::get();
	prtty::term term = prtty::get("screen-256color");

	// you can also specify the base search path, though this usually isn't necessary.
	prtty::term term = prtty::get("screen-256color", "/path/to/termdb");

	cout << "selected terminal: " << term.id << endl;
	if (!term.names.empty()) {
		cout << "also known as:" << endl;

		for (auto &name : term.names) {
			cout << "\t- " << name << endl;
		}
	}
	cout << endl;

	// then refer to man terminfo(5) for values.
	//
	// prtty uses the long-form capability names as properties.
	cout << "this terminal supports up to " << term.max_colors << " colors." << endl;
	cout << "this terminal " << (term.over_strike ? "can" : "cannot") << " overstrike." << endl;
	cout << endl;

	// when it comes to string values, prtty implements the Terminfo scripting language,
	// and thus all string values are both stream manipulators as well as functions.
	//
	// the following outputs "jello is tasty!"
	cout << "hello";
	cout << term.column_address; // arguments default to int(0)
	cout << "j";
	cout << term.parm_right_cursor(5);
	cout << "is tasty!";
	cout << endl;

	return 0;
}
```

On a fairly standard emulator with a `$TERM` value of `xterm-256color`, the above yields:

```
selected terminal: xterm-256color
also known as:
	- xterm-256color
	- xterm with 256 colors

this terminal supports up to 256 colors.
this terminal cannot overstrike.

jello is tasty!
```

## String Capabilities
String capabilities in terminfo descriptions should normally never be used as-is. They are actually
format strings (similar to [`printf`](https://www.lix.polytechnique.fr/~liberti/public/computing/prog/c/C/FUNCTIONS/format.html))
that implement a stack-based set of operators (including if-then-else statements, arithmetic, etc).

Even string values that do not take an argument can still reference a value stored by a previous
argument, whether it be a flag or something else. While this is uncommon, complete portability therefore
cannot rely on the assumption that zero-argument format strings are free of these parameters.

Due to this, prTTY uses streamable types in lieu of `std::string`s. Streaming a string capability
by itself executes the format string with all 9 arguments set to `int(0)`.

You can also call string capabilities, like functions, to populate the arguments beforehand. This
allows for things like `term.parm_right_cursor(5)` above to happen.

Note that arguments are captured _by reference_, so you shouldn't store the result of a string capability
call (e.g. don't do something like `auto moveRight5 = term.parm_right_cursor(5)`.

# License
Licensed under [CC0](LICENSE). Go crazy, but let me know if you use this; it's always appreciated.
