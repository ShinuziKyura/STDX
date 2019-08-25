#include <iostream>
#include <string>

#include "utility.hpp"
#include "functional.hpp"

void func() {}

struct Obj
{
	void operator()() {}
};

struct Test
{
	Test()
		: mo2(mo1)
		, mo3(std::move(mo1))
	{
	}

	void mf1() {};
	
	Obj mo1;
	Obj& mo2;
	Obj&& mo3;
	Obj* mo4;
};

int example_10()
{
	void(& f1)() = func;
	void(&& f2)() = func;
	void(* f3)() = func;

	Obj o1;
	Obj& o2 = o1;
	Obj&& o3 = std::move(o1);
	Obj* o4;

	Test t;

	std::cout << stdx::_implementation::_invoke_semantics<decltype(f1)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(f2)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(f3)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(o1)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(o2)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(o3)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(o4)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(&Test::mf1)>::is_invocable;
	std::cout << stdx::_implementation::_invoke_semantics<decltype(&Test::mo1)>::is_invocable;

//	stdx::xbind(0);

//	std::cout << stdx::_implementation::_is_bindable<stdx::_implementation::_invoke_semantics<int>, stdx::meta::pack<>>::_value;

	stdx::await_input();

	return 0;
}