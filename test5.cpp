#include <iostream>

#include "spin_mutex.hpp"
#include "atomic_ptr.hpp"

int main5()
{
	stdx::spin_mutex mtx;

	mtx.lock();

	std::cout << "Hi planet!";

	mtx.unlock();

	return 0;
}
