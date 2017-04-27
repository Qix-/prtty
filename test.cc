#include <iostream>
#include "./prtty.hpp"

int main() {
	prtty::impl::Data data;
	//prtty::impl::exec(data, std::cout, "This is 100%%\n");
	prtty::impl::exec(data, std::cout, "%p1%Pp%p2%PrWhen I say '%gp%s' you say '%gr%s': %gp%s! (%gr%s!)\n", "Mac", "Daddy");
	return 0;
}
