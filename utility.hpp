#ifndef STDX_UTILITY_HPP
#define STDX_UTILITY_HPP

#include <iostream>
#include <type_traits>

#include "language.hpp"

#if defined(__COUNTER__)
#define STDX_COUNTER __COUNTER__
#else
#define STDX_COUNTER __LINE__
#endif

// Declares an anonymous variable whose lifetime is limited to the scope where it is declared; should be declared only inside functions or at the global scope, one per line
#define STDX_SCOPED_VARIABLE(...) [[maybe_unused]] auto STDX_CONCATENATE(STDX_VARIABLE_, STDX_COUNTER) = __VA_ARGS__

// Marks a class as polymorphic; should be declared in base clause of the class
#define STDX_POLYMORPHIC_CLASS protected virtual stdx::_polymorphic

namespace stdx
{
	class _polymorphic
	{
	protected:
		_polymorphic() = default;
		virtual ~_polymorphic() = default;
	};

	// [Note: Currently calling this function is a no-op --end note] Should be called only once, inside a non-reentrant function, before any IO operation has happened
	inline void unsynced_ios()
	{
	/*	std::setvbuf(stdin, NULL, _IOFBF, BUFSIZ);
		std::setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
		std::setvbuf(stderr, NULL, _IOFBF, BUFSIZ);
		std::ios_base::sync_with_stdio(false); */
	}

	inline void ignoreline()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), std::cin.widen('\n'));
	}

	template <class Type>
	inline void memzero(Type & obj)
	{
		static_assert(std::conjunction_v<std::negation<std::is_const<Type>>, std::is_trivially_copyable<Type>, std::is_standard_layout<Type>>, 
					  "'stdx::memzero(Type &)': Type must be a non-const, trivially copyable, standard layout type!");
		std::memset(&obj, 0, sizeof obj);
	}
}

#endif
