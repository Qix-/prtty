#include "./prtty.hpp"
#include <iostream>

int main() {
	prtty::impl::Data data;
	prtty::impl::Sequence seq = prtty::impl::Sequence::parse("%itranslated coords are %p1%dx%p2%d.");
	seq(data, std::cout);
	return 0;
}
