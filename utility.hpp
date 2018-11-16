#ifndef STDX_UTILITY_HPP
#define STDX_UTILITY_HPP

#include <iostream>
#include <type_traits>

#include "stdx_macros.hpp"

// Declares an anonymous variable whose lifetime is limited to the scope where it is declared; should be declared only inside functions or at the global scope, one per line
#define STDX_SCOPED_VARIABLE(...) STDX_MACRO_FUNCTION_n_ary(STDX_SCOPED_VARIABLE, __VA_ARGS__)
#define STDX_SCOPED_VARIABLE_FUNCTION(context, unique, ...) [[maybe_unused]] auto STDX_MACRO_VARIABLE(variable, context, unique) = __VA_ARGS__

// Marks a class as polymorphic; should be declared in base clause of the class
#define STDX_POLYMORPHIC_CLASS protected virtual ::stdx::_polymorphic

// Function call returns true on the first time execution passes through a particular invocation, and false on next invocations.
#define STDX_ONCE() ::stdx::_once([]{})

namespace stdx
{
	class _polymorphic
	{
	protected:
		_polymorphic() = default;
		virtual ~_polymorphic() = default;
	};

	template <class Type>
	[[nodiscard]] constexpr bool _once(Type)
	{
		static bool once = true; // Would it make more sense for this to be thread_local?
		return std::exchange(once, false);
	}

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
