#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>
#include <type_traits>
#include <thread>
#include <stack>

#include "utility.hpp"

#ifdef STDX_THREAD_SUPPRESS_WARNINGS
#if defined(_MSVC_LANG)
#pragma warning(disable: 4611)
#pragma warning(disable: 4715)
#else
// TODO suppress warnings for other compilers
#endif
#endif

// Checks if previous invoke returned without jumping
#define STDX_THREAD_JUMP_CHECK_INVOKE() \
[]\
{\
	return !::stdx::this_thread::jump_status;\
}\
()

// Checks if next return will jump
#define STDX_THREAD_JUMP_CHECK_RETURN() \
[]\
{\
	return !::stdx::this_thread::jump_env_stack.empty();\
}\
()

// Executes invocation passed as argument while setting a point to return to without actually returning from that function
#define STDX_THREAD_JUMP_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JUMP_INVOKE, invocation)
#define STDX_THREAD_JUMP_INVOKE_(context, unique, invocation) \
[&] () -> decltype(invocation)\
{\
	thread_local int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0;\
	struct STDX_MACRO_TYPE(scoped_thread, context, unique)\
	{\
		~STDX_MACRO_TYPE(scoped_thread, context, unique)()\
		{\
			::stdx::this_thread::jump_env_stack.pop();\
			::stdx::this_thread::jump_status = STDX_MACRO_VARIABLE(status, context, unique);\
		}\
	}\
	volatile STDX_MACRO_VARIABLE(scoped_this_thread, context, unique);\
	if (!(STDX_MACRO_VARIABLE(status, context, unique) = setjmp(::stdx::this_thread::jump_env_stack.emplace())))\
	{\
		return invocation;\
	}\
}\
()

// Returns execution to last point in stack without returning from current function
#define STDX_THREAD_JUMP_RETURN(...) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JUMP_RETURN, __VA_ARGS__)
#define STDX_THREAD_JUMP_RETURN_(context, unique, ...) \
[] (int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0)\
{\
	if (!::stdx::this_thread::jump_env_stack.empty())\
	{\
		::std::longjmp(::stdx::this_thread::jump_env_stack.top(), STDX_MACRO_VARIABLE(status, context, unique));\
	}\
}\
(__VA_ARGS__)

namespace stdx
{
	struct this_thread
	{
		thread_local static inline std::stack<std::jmp_buf> jump_env_stack;
		thread_local static inline int jump_status = 0;
	};
}

namespace stdx::thread
{
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
