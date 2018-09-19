#include <iostream>
#include <shared_mutex>

#define STDX_USING_ALL

#include "spinlock.hpp"
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

bool test(int i)
{
	std::cout << "Evaluated i" << std::endl;
	return i < 10;
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

	while (test(i))
	{
		std::cout << i++ << std::endl;
	}
	else
	{
		std::cout << "Never executed i++!" << std::endl;
	}

#include STDX_DO_WHILE

	do
	{
		std::cout << i-- << std::endl;
		std::cout << "Executed i-- at least once!" << std::endl;
	}
	while (i > 10);
	
	stdx::stopwatch::start();
	mtx.lock_shared();
	std::cout << stdx::stopwatch::split() << std::endl;

	std::cout << "Hi" << ptr.get()->c << "planet!" << std::endl;
	stdx::zeromem(*ptr);

	stdx::stopwatch::split();
	mtx.unlock_shared();
	std::cout << stdx::stopwatch::stop() << std::endl;

	std::cout << stdx::stopwatch::total_time() << std::endl;

	return 0;
}
