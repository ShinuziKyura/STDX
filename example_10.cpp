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
	void mf1() {}
	
	Obj mo1;
	Obj* mo2;
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

	stdx::xbind(f1);
	stdx::xbind(f2);
	stdx::xbind(f3);
	stdx::xbind(o1);
	stdx::xbind(o2);
	stdx::xbind(o3);
	stdx::xbind(o4);
	stdx::xbind(&Test::mf1, t);
	stdx::xbind(&Test::mf1, &t);
	stdx::xbind(&Test::mo1, t);
	stdx::xbind(&Test::mo1, &t);
	stdx::xbind(&Test::mo2, t);
	stdx::xbind(&Test::mo2, &t);

	stdx::await_input();

	return 0;
}