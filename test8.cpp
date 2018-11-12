#include <iostream>
#include <csetjmp>
#include <thread>

#define STDX_THREAD_SUPPRESS_WARNINGS
#include "thread.hpp"

/*

class _index
{
public:
	constexpr _index(int const & value) :
		_lhs(value),
		_rhs(value)
	{
	}

	constexpr operator int const &() const
	{
		return _rhs;
	}
private:
	constexpr _index(int const & lhs, _index const & rhs) :
		_lhs(lhs),
		_rhs(rhs)
	{
	}

	int const & _lhs;
	int const & _rhs;

	friend _index operator,(int const &, _index const &);
};

constexpr _index operator,(int const & lhs, _index const & rhs)
{
	return _index(lhs, rhs);
}

*/

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

		if (&b_ref == &b)
			std::cout << "b(" << b_ref << ")\n";
	}

	return 0;
}