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
constexpr stdx::matrix<float, 3> A1({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });
[[maybe_unused]]
constexpr stdx::matrix<float, 3> A2({ 1, 2, 3,
									4, 5, 6,
									7, 8, 9 });

int example_5()
{
	stdx::desync_IO();

	STDX_DEFINE_STOPWATCH(std::chrono::microseconds);

	////////////////////////////////////////
	/// Matrix test start

//	MSVC can't compile this line, GCC can
//	Which of them is right (if any)?
//	[[maybe_unused]] constexpr stdx::matrix P1 = A1.partial_pivoting();
/*	for (std::size_t i = 1; i <= M1.rows; ++i)
	{
		for (std::size_t j = 1; j <= M1.columns; ++j)
		{
			std::cout << M1(i, j) << " ";
		}
		std::cout << "\n";
	} */

	auto const M1 = A1.LUP_decomposition();

	for (std::size_t i = 1; i <= M1.rows; ++i)
	{
		for (std::size_t j = 1; j <= M1.columns; ++j)
		{
			std::cout << M1(i, j) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	for (std::size_t i = 1; i <= M1.rows; ++i)
	{
		for (std::size_t j = 1; j <= M1.columns; ++j)
		{
			std::cout << M1.lower(i, j) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	for (std::size_t i = 1; i <= M1.rows; ++i)
	{
		for (std::size_t j = 1; j <= M1.columns; ++j)
		{
			std::cout << M1.upper(i, j) << " ";
		}
		std::cout << "\n";
	}
	std::cout << "\n";

	/// Matrix test end
	////////////////////////////////////////

	std::fstream fout("example_5.txt", std::ios::out | std::ios::trunc);

	STDX_SCOPED_VARIABLE(stdx::streamroute<std::ostream, stdx::policy::replicate>(std::cout, fout));

	S s;
	int i = 0;

#include STDX_LANGUAGE_WHILE_ELSE

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

#include STDX_LANGUAGE_DO_WHILE

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
