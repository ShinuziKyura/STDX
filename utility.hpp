#ifndef STDX_header_UTILITY
#define STDX_header_UTILITY

#include <iostream>
#include <type_traits>
#include <atomic>

#include "stdx/core.hpp"

// Declares an anonymous variable whose lifetime is limited to the scope where it is declared; should be declared only inside functions or at the global scope, one per line
#define STDX_SCOPED_VARIABLE(...) STDX_MACRO_FUNCTION_N_ARY(SCOPED_VARIABLE, __VA_ARGS__)
#define STDX_implementation_SCOPED_VARIABLE(context, ...) [[maybe_unused]] auto STDX_MACRO_VARIABLE(variable, context) = __VA_ARGS__

// Marks a class as polymorphic; should be declared in base clause of the class
#define STDX_POLYMORPHIC_CLASS protected virtual ::stdx::_polymorphic

// Function that returns true on first invocation, and false on next invocations; invocation is thread-safe and only the first invocation among multiple threads will return true
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
	[[nodiscard]] bool _once(Type)
	{
		static std::atomic_flag flag = ATOMIC_FLAG_INIT;
		return !flag.test_and_set(std::memory_order_relaxed);
	}

	// Should be called only before any IO operation has happened
	inline void desync_io()
	{
	//	std::setvbuf(stdin, NULL, _IOFBF, BUFSIZ);
	//	std::setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
	//	std::setvbuf(stderr, NULL, _IOFBF, BUFSIZ);
		
		std::ios_base::sync_with_stdio(false);
	}

	inline void await_input()
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
