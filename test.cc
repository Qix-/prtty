#include "./prtty.hpp" // include first so as to not shadow errors with missing header files

#include <iostream>

using namespace std;

int main(int argc, char **argv) {
	(void) argc;

	prtty::term term = argc >= 2
		? prtty::get("xterm-256color", argv[1])
		: prtty::get();

	// test boolean operations
	cout << endl << "has micromovements: " << bool(term.enter_micro_mode) << endl; // usually false
	cout << "has clear screen: " << bool(term.clear_screen) << endl; // usually true
	cout << "has set a (ANSI) foreground: " << bool(term.set_a_foreground) << endl; // true on color terminals
	cout << endl << "colors: " << term.max_colors << endl;

	cout << endl;
	cout << "hello";
	cout << term.column_address; // arguments default to int(0)
	cout << "j";
	cout << term.parm_right_cursor(5);
	cout << "is tasty!";
	cout << endl;

	return 0;
}
