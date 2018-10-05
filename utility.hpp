#ifndef STDX_UTILITY_HPP
#define STDX_UTILITY_HPP

#include <iostream>

#include "language.hpp"

// May be used only in functions, one per line!
#define STDX_SCOPED_VARIABLE(...) STDX_CONCATENATE(STDX_SCOPED_VARIABLE_, __LINE__)(__VA_ARGS__)

namespace stdx
{
	inline void ignoreline()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), std::cin.widen('\n'));
	}
}

#endif
