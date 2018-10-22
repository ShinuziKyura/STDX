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

template <char ... Chars>
constexpr auto operator "" _n()
{
	return stdx::meta::intstring<Chars ...>;
}

int test5()
{
	stdx::unsynced_ios();

	STDX_USING_STOPWATCH(std::chrono::microseconds);

	constexpr stdx::math::matrix<int, 9> m0(stdx::math::matrix_name::pascal_upper);
	[[maybe_unused]]
	constexpr stdx::math::matrix<int, 2> m1({ 1, 2,
											  3, 4 });
	[[maybe_unused]]
	constexpr stdx::math::matrix<int, 2> m2({ 1, 2,
											  3, 4 });
//	MSVC can't compile this, G++ can
//	constexpr stdx::math::matrix m3 = m1 + m2;	
//	constexpr auto e = m3(1, 1);

//	On the other hand, MSVC can compile this, while G++ can't
//	constexpr auto m3 = m1 + m2;

//	Which compiler is right (if any)? 

	for (size_t i = 1; i <= m1.rows(); ++i)
	{
		for (size_t j = 1; j <= m1.columns(); ++j)
		{
			auto const m = m1(i, j);
			std::cout << (j != 1 ? (m < 10 ? "  " : " ") : "") << m;
		}
		std::cout << std::endl;
	}
	std::cout << std::endl;

	std::fstream fout("test5.txt", std::ios::out | std::ios::trunc);
	stdx::stream_traits<std::ostream>::route<stdx::policy::replicate> STDX_SCOPED_VARIABLE(std::cout, fout);

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

	std::cout << "Elapsed time in while-else loop: " << stopwatch::split() << std::endl << std::endl;

	stdx::memzero(s);

#include STDX_DO_WHILE

	stopwatch::split();

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
