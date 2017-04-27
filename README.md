# prTTY

Simplified Terminfo parser and a neat little wrapper/implementation of the terminfo "scripting" language.

Aims to have a similar API as [zachariahreed/pyterminfo](https://github.com/zachariahreed/pyterminfo).
There are several of these out there but they all take shortcuts or don't help you actually _use_ the
values.

## Usage

```c++
#include <iostream>
#include "prtty.hpp"

using namespace std;

int main() {
	// get the current terminal
	prtty::term term = prtty::get();
	// or, to specify a terminal name manually:
	prtty::term term = prtty::get("screen-256color");

	cout << "selected terminal: " << term.id << endl;
	if (!term.names.empty()) {
		cout << "also known as:" << endl;
		for (auto &name : term.names) {
			cout << "\t- " << name << endl;
		}
	}
	cout << endl;

	// then refer to man terminfo(5) for values.
	// prtty uses the long-form value names.
	// ref: https://gist.github.com/rwboyer/1691527#file-openbsd-terminfo-L106-L588
	cout << "this terminal supports up to " << term.max_colors << " colors." << endl;
	cout << "this terminal " << (term.over_strike ? "can" : "cannot") << " overstrike." << endl;
	cout << endl;

	// when it comes to string values, prtty implements the Terminfo scripting language,
	// and thus all string values are both stream manipulators as well as functions.
	cout << "hello";
	cout << term.column_address(); // arguments default to int(0)
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

# License
Licensed under [CC0](LICENSE). Go crazy, but let me know if you use this; it's always appreciated.
