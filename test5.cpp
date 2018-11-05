#include <iostream>

#define STDX_USING_ALL

#include "meta.hpp"
#include "language.hpp"
#include "utility.hpp"
#include "matrix.hpp"

//#define STDX_STOPWATCH_RESOLUTION std::chrono::microseconds
#include "stopwatch.hpp"
#include "stream.hpp"

struct S // : STDX_POLYMORPHIC_CLASS
{
	int i = 1;
	double d = 0.5;
	char c = ' ';
};

namespace
{
	bool test_less(int i)
	{
		bool test = i < 10;
		std::cout << std::boolalpha << "Evaluated i < 10: " << test << std::endl;
		return test;
	}

	bool test_greater(int i)
	{
		bool test = i > 0;
		std::cout << std::boolalpha << "Evaluated i > 0: " << test << std::endl;
		return test;
	}
}
template <char ... Chars>
constexpr auto operator "" _n()
{
	return stdx::meta::intstring<Chars ...>;
}

[[maybe_unused]]
constexpr stdx::matrix<int, 3> m1({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });
[[maybe_unused]]
constexpr stdx::matrix<int, 3> m2({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });

int test5()
{
	stdx::unsynced_ios();

	STDX_DEFINE_STOPWATCH(std::chrono::microseconds);

	constexpr auto pm = m1.permutate();

//	VC++ can't compile this but G++ can
//	constexpr stdx::matrix m3 = m1 * m2;
//	constexpr auto e3 = m3(1, 1);

	std::fstream fout("test5.txt", std::ios::out | std::ios::trunc);

	STDX_SCOPED_VARIABLE(stdx::streamroute<std::ostream, stdx::policy::replicate>(std::cout, fout));

	S s;
	int i = 0;

#include STDX_WHILE_ELSE

	stopwatch::start();

	while (test_less(i))
	{
		std::cout << ++i << std::endl;
	}
	else
	{
		std::cout << "Never executed loop!" << std::endl;
	}

	std::cout << "Elapsed time in while-else loop: " << stopwatch::stop() << std::endl << std::endl;

	stdx::memzero(s);

#include STDX_DO_WHILE

	stopwatch::start();

	do
	{
		std::cout << --i << std::endl;
	}
	while (test_greater(i));

	std::cout << "Elapsed time in do-while loop: " << stopwatch::stop() << std::endl << std::endl;

	std::cout << "Elapsed time in total: " << stopwatch::total_time() << std::endl;

	fout.close();

	stdx::ignoreline();

	return 0;
}
