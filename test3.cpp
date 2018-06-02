#include <iostream>
#include <functional>
#include <future>

#include "meta.hpp"

template <class Type>
struct pointer_type
{
	using type = Type *;
};

#pragma warning(disable: 4297)

long func(char, int) noexcept
{
	return 1;
}

struct Class
{
	void func(double, int) const volatile & noexcept
	{
	}
};

struct Func
{
	template <class FuncType>
	Func(FuncType *)
	{
		std::cout << "1\n" << typeid(FuncType).name() << "\n";
	}
	template <class FuncType, class ObjType>
	Func(FuncType ObjType::*)
	{
		std::cout << "2\n" << typeid(FuncType).name() << "\n";
	}
};

long (*f)(char, int) noexcept = &func;

void (Class::* cf)(double, int) const volatile & noexcept = &Class::func;

int main()
{
	using namespace stdx::meta;

	[[maybe_unused]] constrained_pack<std::is_placeholder, pack<pack<int &, int>, pack<double, double>, pack<decltype(std::placeholders::_1), bool>, pack<char, char &&>>> t1;

	[[maybe_unused]] transformed_pack<pointer_type, pack<pack<unsigned int, Class, double>, pack<float, bool>>> t2;

	[[maybe_unused]] permutated_pack<pack<short, int, long, long long>, valpack<1, 3, 0, 2>> t3;

	[[maybe_unused]] constrained_pack<value_trait<beside<2, 4>::trait>::value, as_pack_val<valpack<0, 2, 5, 6, 3, 4, 1, 3>>> t4;

	[[maybe_unused]] transformed_pack<std::is_class, pack<Class, long, unsigned char>> t5;

	[[maybe_unused]] permutated_pack<pack<pack<double, long double, pack<val<1>, int>>, pack<char, val<2>, bool>, val<3>>, pack<val<2>, val<0>, val<1>>> t6;

	[[maybe_unused]] merged_pack<pack<int, double, char>, pack<int, double, char>> t7;

	[[maybe_unused]] transformed_pack<type_trait<std::is_placeholder>::first, pack<pack<int &, int>, pack<double, double>, pack<decltype(std::placeholders::_1), bool>, pack<char, char &&>>> t8;

	type_cast<int, float>();

//	type_cast<int, Class>(); // SFINAE error if not convertible

	return 0;
}
