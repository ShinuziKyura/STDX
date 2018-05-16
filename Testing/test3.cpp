#include <iostream>
#include <functional>

//#include "concurrent_queue.hpp"
#include "templates.hpp"

int main()
{
	{
	//	stdx::atomic_ptr<int> aptr(new int(3));
	}

	[[maybe_unused]] 
	stdx::templates::constrained_typetuple<std::is_placeholder, stdx::templates::typetuple<
		stdx::templates::typepair<int &, int>, 
		stdx::templates::typepair<double, double>, 
		stdx::templates::typepair<decltype(std::placeholders::_1), bool>,
		stdx::templates::typepair<char, char &&>
	>> t1;

	[[maybe_unused]]
	stdx::templates::constrained_valuetuple<stdx::templates::range<2, 4>::between, stdx::templates::valuetuple<0, 2, 5, 6, 3, 4, 1, 3>> t2;

	[[maybe_unused]]
	stdx::templates::constrained_typevaluetuple<std::is_integral, stdx::templates::typetuple<
		stdx::templates::typevalue<int, 1>, 
		stdx::templates::typevalue<int, 3>, 
		stdx::templates::typevalue<double, 2>
	>> t3;

	[[maybe_unused]]
	stdx::templates::reordered_tuple<stdx::templates::typetuple<short, int, long, long long>, stdx::templates::valuetuple<1, 3, 0, 2>> t4;

	return 0;
}
