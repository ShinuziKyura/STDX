#include <iostream>

#define STDX_USING_ALL

#include "meta.hpp"
#include "language.hpp"
#include "utility.hpp"

#include "stopwatch.hpp"
#include "stream.hpp"

//#define _ENABLE_ATOMIC_ALIGNMENT_FIX
//#include "atomic_ptr.hpp"

struct S
{
//	virtual ~S() = default;
//private:
	int i = 1;
	double d = 0.5;
	char c = ' ';
};

bool test_less(int i)
{
	bool test = i < 10;
	std::cout << std::boolalpha << "Evaluated i: " << test << std::endl;
	return test;
}

bool test_greater(int i)
{
	bool test = i > 0;
	std::cout << std::boolalpha << "Evaluated i: " << test << std::endl;
	return test;
}

int main()
{
	stdx::faster_ios();

	std::fstream fout("test.txt", std::ios::out | std::ios::trunc);
	stdx::streamroute<stdx::policy::repeat> STDX_UNNAMED_VARIABLE(std::cout, fout);

	S s;
	int i = 0;

#include STDX_WHILE_ELSE

	stdx::stopwatch::start();

	while (test_less(i))
	{
		std::cout << ++i << std::endl;
	}
	else
	{
		std::cout << "Never executed ++i!" << std::endl;
	}

	std::cout << "Elapsed time in while-else loop: " << stdx::stopwatch::split() << std::endl << std::endl;

	stdx::memzero(s);

#include STDX_DO_WHILE

	stdx::stopwatch::split();

	do
	{
		std::cout << --i << std::endl;
	}
	while (test_greater(i));

	std::cout << "Elapsed time in do-while loop: " << stdx::stopwatch::stop() << std::endl << std::endl;

	std::cout << "Elapsed time in total: " << stdx::stopwatch::total_time() << std::endl;

	fout.close();

	stdx::ignoreline();

	return 0;
}
