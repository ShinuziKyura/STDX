#include <iostream>
#include <csetjmp>
#include <thread>

#define STDX_THREAD_SUPPRESS_WARNINGS
#include "thread.hpp"
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
		STDX_THREAD_JUMP_RETURN();
	}

	return b;
}

int test8()
{
	int count = 0;
	
	while (count != 9)
	{
		int & b_ref = STDX_THREAD_JUMP_INVOKE(a(++count));
		
		if (STDX_THREAD_JUMP_CHECK_INVOKE())
			std::cout << "b(" << b_ref << ")\n";
	}

	auto var = STDX_THREAD_JUMP_VARIABLE(int, 0);

	std::cout << var;
	
	STDX_THREAD_JUMP_INVOKE(func());

	return 0;
}