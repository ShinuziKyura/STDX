#include <functional>
#include <future>

#include "meta.hpp"

struct Class
{
};

template <class ...>
struct ClassTemplate
{
};

int example_3()
{

	[[maybe_unused]] stdx::meta::constrained_pack<stdx::meta::apply_to_pack_element<std::is_placeholder>::first, stdx::meta::pack<stdx::meta::pack<int &, int>, stdx::meta::pack<double, double>, stdx::meta::pack<decltype(std::placeholders::_1), bool>, stdx::meta::pack<char, char &&>>> t1;

	[[maybe_unused]] stdx::meta::transformed_pack<std::add_const, stdx::meta::pack<stdx::meta::pack<unsigned int, Class, double>, stdx::meta::pack<float, bool>>> t2;

	[[maybe_unused]] stdx::meta::permutated_pack<stdx::meta::pack<short, int, long, long long>, stdx::meta::valpack<1, 3, 0, 2>> t3;

	[[maybe_unused]] stdx::meta::constrained_pack<stdx::meta::apply_to_value<stdx::meta::outside<2, 4>::exclusive>::trait, stdx::meta::as_pack_val<stdx::meta::valpack<0, 2, 5, 6, 3, 4, 1, 3>>> t4;

	[[maybe_unused]] stdx::meta::transformed_pack<std::is_class, stdx::meta::pack<Class, long, unsigned char>> t5;

	[[maybe_unused]] stdx::meta::permutated_pack<stdx::meta::pack<stdx::meta::pack<double, long double, stdx::meta::pack<stdx::meta::val<1>, int>>, stdx::meta::pack<char, stdx::meta::val<2>, bool>, stdx::meta::val<3>>, stdx::meta::pack<stdx::meta::val<2>, stdx::meta::val<0>, stdx::meta::val<1>>> t6;

	[[maybe_unused]] stdx::meta::transposed_pack<stdx::meta::pack<int, double, char>, stdx::meta::pack<int, double, char>> t7;

	using f1 = stdx::meta::bind<ClassTemplate, stdx::meta::placeholders::_3, bool, stdx::meta::placeholders::_1, stdx::meta::placeholders::_1>;

	using r1 = f1::invoke<int, char, double, double, unsigned>;

	[[maybe_unused]] stdx::meta::make_integer_sequence<int, -2, 3> seq;

	stdx::meta::type_if<1 + 2 == 2>::then<
		int
	>::else_if<1 + 2 == 4>::then<
		double
	>::else_then<
		char
	>::end_if();

	return 0;
}
