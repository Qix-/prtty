#include "./prtty.hpp"
#include <iostream>

using namespace std;

int main() {
	prtty::term term = prtty::get();
	cout << "detected terminal: " << term.id << endl;
	cout << "common names:" << endl;
	for (auto name : term.names) {
		cout << "\t- " << name << endl;
	}

	//ios::fmtflags f(cout.flags());
	//#define PRTTY_DO_STRING(name) {cout << prtty::impl::Sequence::parse(term.name).nargs << "\t" << std::setw(30) << std::left << #name << term.name << endl; cout.flags(f);}
	//#include "./prtty-strings.inc"
	//#undef PRTTY_DO_STRING

	prtty::impl::Data data;
	auto seq = prtty::impl::Sequence::parse("%p1%p2%-%Px%p3%p4%*%gx%+%:-+10d!");
	seq(data, cout, 10, 5, 6, 5); // should be "+35       !"
	cout << endl;

	return 0;
}
