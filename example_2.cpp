#include <iostream>
#include <future>

#define SITUATION 1

void func()
{
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int example_2()
{
	auto start = std::chrono::steady_clock::now();

#if SITUATION == 1 // 3 seconds
	{
		auto future = std::async(std::launch::async, func); // Destructor is called
	}
#elif SITUATION == 2 // 3 seconds
	std::async(std::launch::async, func); // Destructor is called
#elif SITUATION == 3 // 2 seconds
	auto future = std::async(std::launch::async, func); // Destructor is not called
#endif

	std::this_thread::sleep_for(std::chrono::seconds(2));

	auto end = std::chrono::steady_clock::now();

	std::cout << std::chrono::duration<double>(end - start).count() << std::endl;

	return 0;
}
