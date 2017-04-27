#include "./prtty.hpp"
#include <iostream>

using namespace std;

int main() {
	prtty::term term = prtty::get();
	cout << term.clear_screen;
	return 0;
}
