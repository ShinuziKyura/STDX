#include <iostream>

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
	if (count != b)
	{
		std::cout << "a(" << count << ")\n";
		STDX_FLOW_RETURN();
	}

	return b;
}

int test8()
{
	int count = 0;
	
	while (count != 9)
	{
		int & b_ref = STDX_FLOW_WAYPOINT_INVOKE(a(++count));
		
		if (STDX_FLOW_CHECK_INVOKE())
			std::cout << "b(" << b_ref << ")\n";
	}
	
	STDX_FLOW_WAYPOINT_INVOKE(func());

	return 0;
}