#ifndef STDX_header_UTILITY
#define STDX_header_UTILITY

#include <type_traits>
#include <iostream>
#include <atomic>

#include "core/stdx.hpp"

// Declares an anonymous variable whose lifetime is limited to the scope where it is declared; should be declared only at block scope
#define STDX_SCOPED_VARIABLE(...) STDX_MACRO_FUNCTION_N_ARY(SCOPED_VARIABLE, __VA_ARGS__)
#define STDX_implementation_SCOPED_VARIABLE(context, ...) [[maybe_unused]] auto STDX_MACRO_VARIABLE(variable, context) = __VA_ARGS__

// Marks a class as polymorphic; should be declared in base clause of the class
#define STDX_POLYMORPHIC_CLASS protected virtual ::stdx::_polymorphic

// Function that returns true on first invocation at a particular callsite, and false on next invocations at the same callsite; invocation is thread-safe and only the first invocation among multiple threads will return true (threads will not synchronize)
#define STDX_ONCE() ::stdx::_once([]{})

#define STDX_UNUSED_PARAM(arg) do{ (arg, void()); }while(0)
#define STDX_UNUSED_PARAM_PACK(args) do{ ((args, void()), ...); }while(0)

namespace stdx
{
	class _polymorphic
	{
	protected:
		_polymorphic() noexcept = default;
		virtual ~_polymorphic() noexcept = default;

	};

	// Any_type, empty type that can be default-constructed and has constructor that accepts any number of arguments of any type

	class any_type
	{
	public:
		template <class ... Types>
		any_type(Types && ...) noexcept
		{
		}

	};

	template <class Type>
	[[nodiscard]] bool _once(Type const &) noexcept
	{
		static std::atomic_flag flag = ATOMIC_FLAG_INIT;
		return !flag.test_and_set(std::memory_order_relaxed);
	}

	// Should be called only before any IO operation has happened
	inline void buffer_IO()
	{
		std::setvbuf(stdout, NULL, _IOFBF, BUFSIZ);
		std::setvbuf(stderr, NULL, _IOFBF, BUFSIZ);
	}

	// Should be called only before any IO operation has happened
	inline void desync_IO()
	{
		std::ios_base::sync_with_stdio(false);
	}

	inline void await_input()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), std::cin.widen('\n'));
	}

	template <class Type>
	void memzero(Type & obj) noexcept
	{
		static_assert(std::conjunction_v<std::negation<std::is_const<Type>>, std::is_trivially_copyable<Type>, std::is_standard_layout<Type>>, 
					  "'stdx::memzero(Type &)': Type must be a non-const, trivially copyable, standard layout type!");
		std::memset(&obj, 0, sizeof obj);
	}
}

#endif
