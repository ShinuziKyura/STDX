#include <iostream>

#define STDX_FLOW_SUPPRESS_WARNINGS
#include "flow.hpp"
#include "utility.hpp"

struct S
{
	~S()
	{
		std::cout << "Destructed\n";
	}
};

namespace
{
	void func()
	{
		stdx::await_input();
	}
}

int b = 5;

int & a(int count)
{
	STDX_FLOW_SCOPE()
	if (count != b)
	{
		auto & s = STDX_FLOW_DECLARE(S());
		std::cout << "Will jump - a(" << count << ")\n";
		STDX_FLOW_JUMP();
	}

	std::cout << "Will not jump - ";

	return b;
}

int example_8()
{
	int count = 0;
	
	while (count != 9)
	{
		int & b_ref = STDX_FLOW_SET_AND_INVOKE(a(++count));
		
		if (STDX_FLOW_STATUS() == 0)
		{
			std::cout << "b(" << b_ref << ")\n";
		}
	}

	STDX_FLOW_SET_AND_INVOKE(func());
	
	return 0;
}