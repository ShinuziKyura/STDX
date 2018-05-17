#include <iostream>
#include <array>
#include <vector>
#include <list>

#include "random_generator.hpp"
#include "generative_iterator.hpp"

int main0()
{
	std::array<short, 5>				arr = { 0, 0, 0, 0, 0 };
	std::vector<long>					vec = { 0, 0, 0, 0, 0 };
	std::list<float>					lis = { 0, 0, 0, 0, 0 };

	stdx::random_uniform_int_generator	rg;

	std::cout << "Array\n";
	for (size_t index = 0; index < arr.size(); ++index)
		std::cout << "index: " << index << " | element: " << arr[index] << "\n";

	std::cout << "\nVector\n";
	for (size_t index = 0; index < vec.size(); ++index)
		std::cout << "index: " << index << " | element: " << vec[index] << "\n";

	std::cout << "\nList\n";
	for (size_t index = 0; index < lis.size(); ++index)
		std::cout << "index: " << index << " | element: " << *std::next(lis.begin(), index) << "\n";
	
	std::cout << "\nArray\n";
	{
		auto it_b = stdx::gen_begin(arr, std::ref(rg)),
			 it_e = stdx::gen_end(arr),
			 it = it_b;

		while (it != it_e)
			std::cout << *it++ << "\n";

		std::cout << "\n";

		while (it != it_b)
			std::cout << *--it << "\n";
	}

	std::cout << "\nVector\n";
	{
		auto it_rb = stdx::gen_rbegin(vec, std::ref(rg)),
			 it_re = stdx::gen_rend(vec),
			 it = it_rb;

		while (it != it_re)
			std::cout << *it++ << "\n";

		std::cout << "\n";

		while (it != it_rb)
			std::cout << *--it << "\n";
	}

	std::cout << "\nList\n";
	{
		auto it_b = stdx::gen_begin(lis, std::ref(rg)),
			 it_e = stdx::gen_end(lis),
			 it = it_b;

		while (it != it_e)
			std::cout << *it++ << "\n";

		std::cout << "\n";

		while (it != it_b)
			std::cout << *--it << "\n";
	}

	std::cout << "\nArray\n";
	for (size_t index = 0; index < arr.size(); ++index)
		std::cout << "index: " << index << " | element: " << arr[index] << "\n";

	std::cout << "\nVector\n";
	for (size_t index = 0; index < vec.size(); ++index)
		std::cout << "index: " << index << " | element: " << vec[index] << "\n";

	std::cout << "\nList\n";
	for (size_t index = 0; index < lis.size(); ++index)
		std::cout << "index: " << index << " | element: " << *std::next(lis.begin(), index) << "\n";

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	return 0;
}
