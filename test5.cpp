#include <iostream>
#include <shared_mutex>

#define STDX_USING_ALL

#include "stream.hpp"
#include "spin_mutex.hpp"
#include "stopwatch.hpp"
#include "zeromem.hpp"

#include "language.hpp"

// WIP
#include "atomic_ptr.hpp"
#include "matrix.hpp"

struct S
{
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
	bool test = i > 10;
	std::cout << std::boolalpha << "Evaluated i: " << test << std::endl;
	return test;
}

int main()
{
	using namespace stdx::indexing;
	
	stdx::matrix<int> m(10, 10, stdx::matrix_type::pascal);
	stdx::spin_shared_mutex mtx;
	stdx::atomic_ptr<S> ptr(new S());

//	std::cout << m[8_i,2_j] << std::endl;

	int i = 0;

#include STDX_WHILE_ELSE

	while (test_less(i))
	{
		std::cout << ++i << std::endl;
	}
	else
	{
		std::cout << "Never executed ++i!" << std::endl;
	}

	std::cout << std::endl;

#include STDX_DO_WHILE

	do
	{
		std::cout << --i << std::endl;
	}
	while (test_greater(i));
	
	std::cout << std::endl;

	stdx::stopwatch::start();
	mtx.lock_shared();
	std::cout << stdx::stopwatch::split() << std::endl;

	std::cout << "Hi" << ptr.get()->c << "planet!" << std::endl;
	stdx::zeromem(*ptr);

	stdx::stopwatch::split();
	mtx.unlock_shared();
	std::cout << stdx::stopwatch::stop() << std::endl;

	std::cout << stdx::stopwatch::total_time() << std::endl;

	stdx::wait_for_newline();

	return 0;
}
