#ifndef STDX_UTILITY_HPP
#define STDX_UTILITY_HPP

#include <iostream>
#include <type_traits>

#include "language.hpp"

// May be declared only inside functions or at the global scope, one per line
#define STDX_UNNAMED_VARIABLE(...) STDX_CONCATENATE(STDX_UNNAMED_VARIABLE_, __LINE__)(__VA_ARGS__)

namespace stdx
{
	// May be called only once, inside a non-reentrant function, before any IO operation has happened
	inline void faster_ios()
	{
	//	std::setvbuf(stdin, NULL, _IOFBF, BUFSIZ);
		std::setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
		std::setvbuf(stderr, NULL, _IOFBF, BUFSIZ);
		std::ios_base::sync_with_stdio(false);
	}

	inline void ignoreline()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), std::cin.widen('\n'));
	}

	template <class Type>
	inline void memzero(Type & obj)
	{
		static_assert(std::conjunction_v<std::negation<std::is_const<Type>>, std::is_trivially_copyable<Type>, std::is_standard_layout<Type>>, 
					  "stdx::memzero<Type>: Type must be a non-const, trivially copyable, standard layout type!");
		std::memset(&obj, 0, sizeof obj);
	}
}

#endif
