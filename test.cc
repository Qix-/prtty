#include "./prtty.hpp"
#include <iostream>

using namespace std;

int main() {
	prtty::term term = prtty::get();
	cout
		<< term.clear_screen
		<< "hello"
		<< term.cursor_address(0, 0) << "j"
		<< term.cursor_address(0, 6) << "is tasty!"
		<< endl;

	// test boolean operations
	cout << endl << "has micromovements: " << bool(term.enter_micro_mode) << endl; // usually false
	cout << "has clear screen: " << bool(term.clear_screen) << endl; // usually true
	cout << "has set a (ANSI) foreground: " << bool(term.set_a_foreground) << endl; // true on color terminals
	cout << endl << "colors: " << term.max_colors << endl;
	return 0;
}
