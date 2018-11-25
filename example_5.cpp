#include <iostream>

#define STDX_USING_ALL

#include "meta.hpp"
#include "language.hpp"
#include "utility.hpp"
#include "matrix.hpp"

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
		bool example_ = i < 10;
		std::cout << std::boolalpha << "Evaluated i < 10: " << example_ << std::endl;
		return example_;
	}

	bool test_greater(int i)
	{
		bool example_ = i > 0;
		std::cout << std::boolalpha << "Evaluated i > 0: " << example_ << std::endl;
		return example_;
	}
}
template <char ... Chars>
constexpr auto operator "" _n()
{
	return stdx::meta::intstring<Chars ...>;
}

[[maybe_unused]]
constexpr stdx::matrix<int, 3> A1({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });
[[maybe_unused]]
constexpr stdx::matrix<int, 3> A2({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });

int example_5()
{
	stdx::desync_io();

	STDX_DEFINE_STOPWATCH(std::chrono::microseconds);

	stdx::matrix P1 = A1.pivot();

//	VC++ can't compile this but G++ can
//	constexpr stdx::matrix m3 = m1 * m2;
//	constexpr auto e3 = m3(1, 1);

	for (std::size_t i = 1; i <= P1.rows; ++i)
	{
		for (std::size_t j = 1; j <= P1.columns; ++j)
		{
			std::cout << P1(i, j) << " ";
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::fstream fout("example_5.txt", std::ios::out | std::ios::trunc);

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

	stdx::await_input();

	return 0;
}
