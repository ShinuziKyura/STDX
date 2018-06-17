#include <iostream>

#include "spin_mutex.hpp"

int main5()
{
	stdx::spin_mutex mtx;

	mtx.lock();

	std::cout << "Hi planet!";

	mtx.unlock();

	return 0;
}
