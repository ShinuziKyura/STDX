#include <iostream>
#include <shared_mutex>

#define STDX_USING_MUTEX
#include "spin_mutex.hpp"
#include "spin_shared_mutex.hpp"
#include "synchronized_ptr.hpp"

#include "stopwatch.hpp"
#include "matrix.hpp"

#include "zeromem.hpp"

struct S
{
	int i = 1;
	double d = 0.5;
	char c = ' ';
};

int main()
{
	using namespace stdx::indexing;

	stdx::matrix<int> m(10, 10, stdx::matrix_type::pascal);
	stdx::spin_shared_mutex mtx;
	stdx::synchronized_ptr<S> ptr(new S());

//	std::cout << m[8_i,2_j] << std::endl;

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
