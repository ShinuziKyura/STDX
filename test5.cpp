#include <iostream>

#include "stopwatch.hpp"

#include "spin_mutex.hpp"
#include "atomic_ptr.hpp"

#include "zeromem.hpp"

struct S
{
	int i = 1;
	double d = 1.0;
	char c = 'a';
};

int main()
{
	stdx::spin_mutex mtx;

	stdx::stopwatch::start();
	mtx.lock();
	std::cout << stdx::stopwatch::split() << std::endl;

	std::cout << "Hi planet!\n";
	S s;
	stdx::zeromem(s);

	stdx::stopwatch::split();
	mtx.unlock();
	std::cout << stdx::stopwatch::stop() << std::endl;

	std::cout << stdx::stopwatch::total_time() << std::endl;

	return 0;
}
