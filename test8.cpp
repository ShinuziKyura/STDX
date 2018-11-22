#include <iostream>
#include <any>

#define STDX_FLOW_SUPPRESS_WARNINGS
#include "flow.hpp"
#include "utility.hpp"

namespace
{
	void func()
	{
		stdx::ignoreline();
	}
}

int b = 5;

int & a(int count)
{
	STDX_FLOW_SCOPE()
	if (count != b)
	{
		auto & count_any = STDX_FLOW_DECLARE(std::any(count));
		std::cout << "Will jump - a(" << std::any_cast<int>(count_any) << ")\n";
		STDX_FLOW_JUMP();
	}

	std::cout << "Will not jump - ";

	return b;
}

int test8()
{
	int count = 0;
	
	while (count != 9)
	{
		int & b_ref = STDX_FLOW_JUMP_POINT(a(++count));
		
		if (STDX_FLOW_JUMP_STATUS())
		{
			std::cout << "b(" << b_ref << ")\n";
		}
	}

	STDX_FLOW_JUMP_POINT(func());
	
	return 0;
}