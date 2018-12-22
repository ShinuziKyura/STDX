#include <iostream>

#include "core/stdx.hpp"

#include STDX_DIRECTIVE(flow, suppress_warnings)
#include "flow.hpp"

#include "utility.hpp"

namespace
{
	void func(bool jump)
	{
		if (jump)
		{
			STDX_FLOW_JUMP();
		}

		stdx::await_input();
	}
}

int b = 5;

int & a(int count)
{
	if (count != b)
	{
		std::cout << "a(" << count << ")\n";
		
		STDX_FLOW_INVOKE(func(true));

	//	std::cout << "Will never execute!"; // Causes 'unreachable code' warning
	}

	return b;
}

int example_8()
{
	stdx::desync_io();

	int count = 0;

	while (count != 9)
	{
		int & b_ref = STDX_FLOW_SET_AND_INVOKE(a(++count));
		
		if (STDX_FLOW_STATUS() == 0)
		{
			std::cout << "b(" << b_ref << ")\n";
		}
	}

	func(false);

	return 0;
}
