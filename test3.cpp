#include <iostream>
#include <functional>
#include <future>

#define STDX_USING_META
#include "meta.hpp"

struct Class
{
};

template <class ...>
struct ClassTemplate
{
};

int main3()
{
	[[maybe_unused]] stdx::constrained_pack<stdx::pack_trait<std::is_placeholder>::first, stdx::pack<stdx::pack<int &, int>, stdx::pack<double, double>, stdx::pack<decltype(std::placeholders::_1), bool>, stdx::pack<char, char &&>>> t1;

	[[maybe_unused]] stdx::transformed_pack<std::add_const, stdx::pack<stdx::pack<unsigned int, Class, double>, stdx::pack<float, bool>>> t2;

	[[maybe_unused]] stdx::permutated_pack<stdx::pack<short, int, long, long long>, stdx::valpack<1, 3, 0, 2>> t3;

	[[maybe_unused]] stdx::constrained_pack<stdx::value_trait<stdx::outside<2, 4>::trait>::value, stdx::as_pack_val<stdx::valpack<0, 2, 5, 6, 3, 4, 1, 3>>> t4;

	[[maybe_unused]] stdx::transformed_pack<std::is_class, stdx::pack<Class, long, unsigned char>> t5;

	[[maybe_unused]] stdx::permutated_pack<stdx::pack<stdx::pack<double, long double, stdx::pack<stdx::val<1>, int>>, stdx::pack<char, stdx::val<2>, bool>, stdx::val<3>>, stdx::pack<stdx::val<2>, stdx::val<0>, stdx::val<1>>> t6;

	[[maybe_unused]] stdx::merged_pack<stdx::pack<int, double, char>, stdx::pack<int, double, char>> t7;

	using f1 = stdx::bind<ClassTemplate, stdx::placeholders::_2, bool, stdx::placeholders::_1, stdx::placeholders::_1>;

	using r1 = f1::invoke<int, char, double, double, unsigned>;

	stdx::type_if<1 + 2 == 2>::then<
		int
	>::else_if<1 + 2 == 4>::then<
		double
	>::else_then<
		char
	>::endif();

	stdx::type_cast<int, float>();

//	stdx::type_cast<int, Class>(); // SFINAE error if not convertible

	return 0;
}
