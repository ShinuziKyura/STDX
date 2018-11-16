#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>
#include <type_traits>
#include <thread>
#include <stack>

#include "meta.hpp"
#include "stdx_macros.hpp"

#ifdef STDX_THREAD_SUPPRESS_WARNINGS
#if defined(_MSVC_LANG)
#pragma warning(disable: 4611)
#pragma warning(disable: 4715)
#else
// TODO suppress warnings for other compilers
#endif
#endif

#define STDX_THREAD_JMP_VARIABLE(...) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JMP_VARIABLE, __VA_ARGS__)
#define STDX_THREAD_JMP_VARIABLE_FUNCTION(context, unique, type, ...) ::stdx::thread::_jmp_var<type>(__VA_ARGS__); [[maybe_unused]] ::stdx::thread::_jmp_scoped_variable<type> STDX_MACRO_VARIABLE(jmp, context, unique);

// Checks if previous invoke returned without jumping
#define STDX_THREAD_JMP_CHECK_INVOKE() \
[]\
{\
	return ::stdx::this_thread::jmp_status == 0;\
}\
()

// Checks if next return will jump
#define STDX_THREAD_JMP_CHECK_RETURN() \
[]\
{\
	return !::stdx::this_thread::jmp_env_stack.empty();\
}\
()

// Executes invocation passed as argument while setting a point to return to without actually returning from that function
#define STDX_THREAD_JMP_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JMP_INVOKE, invocation)
#define STDX_THREAD_JMP_INVOKE_FUNCTION(context, unique, invocation) \
[&] () -> decltype(invocation)\
{\
	thread_local int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0;\
	struct STDX_MACRO_TYPE(scoped_thread, context, unique)\
	{\
		~STDX_MACRO_TYPE(scoped_thread, context, unique)()\
		{\
			::stdx::this_thread::jmp_env_stack.pop();\
			::stdx::this_thread::jmp_status = STDX_MACRO_VARIABLE(status, context, unique);\
		}\
	}\
	volatile STDX_MACRO_VARIABLE(scoped_this_thread, context, unique);\
	if ((STDX_MACRO_VARIABLE(status, context, unique) = setjmp(::stdx::this_thread::jmp_env_stack.emplace())) == 0)\
	{\
		return invocation;\
	}\
}\
()

// Returns execution to last point in stack without returning from current function
#define STDX_THREAD_JMP_RETURN(...) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JMP_RETURN, __VA_ARGS__)
#define STDX_THREAD_JMP_RETURN_FUNCTION(context, unique, ...) \
[] (int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0)\
{\
	if (!::stdx::this_thread::jmp_env_stack.empty())\
	{\
		::std::longjmp(::stdx::this_thread::jmp_env_stack.top(), STDX_MACRO_VARIABLE(status, context, unique));\
	}\
}\
(__VA_ARGS__)

namespace stdx
{
	struct this_thread
	{
		thread_local static inline std::stack<std::jmp_buf> jmp_env_stack;
		thread_local static inline int jmp_status = 0;
	};
}

namespace stdx::thread
{
	class _jmp_lifetime // Consider different names for these
	{
		// TODO

		template <class>
		friend class _jmp_scoped_variable;
		template <class>
		friend class _jmp_variable; 
	};

	template <class Type>
	class _jmp_scoped_variable
	{
		// TODO
	};

	template <class Type>
	class _jmp_variable : public Type
	{
		// TODO
		template <class ... ArgTypes>
		_jmp_variable(ArgTypes && ... args) :
			Type(std::forward<ArgTypes>(args) ...)
		{
		}
	};

	template <class Type, class ... ArgTypes>
	constexpr decltype(auto) _jmp_var(ArgTypes && ... args) // decltype(auto) vs auto &&
	{
		static_assert(!std::is_final_v<Type>, "STDX_THREAD_JMP_VARIABLE(type, args ...): type must be a non-final class!");
		if constexpr (std::is_class_v<Type> && !std::is_trivially_destructible_v<Type>)
		{
			// TODO actually construct this in struct that controls its lifetime
			return _jmp_variable<Type>(std::forward<ArgTypes>(args) ...);
		}
		else
		{
			return Type(std::forward<ArgTypes>(args) ...);
		}
	}

	template <class FuncType, class ... ArgTypes>
	void daemon(FuncType func, ArgTypes && ... args)
	{
		std::thread(func, std::forward<ArgTypes>(args) ...).detach();
	}
}

#endif

//=====

#if defined(STDX_USING_THREAD) || defined(STDX_USING_ALL)

namespace stdx
{ 
	using namespace thread;
}

#endif
