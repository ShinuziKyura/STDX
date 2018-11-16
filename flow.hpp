#ifndef STDX_FLOW_HPP
#define STDX_FLOW_HPP

#include <type_traits>

#include "thread.hpp"
#include "stdx_macros.hpp"

#ifdef STDX_FLOW_SUPPRESS_WARNINGS
	#if defined(_MSVC_LANG)
		#pragma warning(disable: 4611)
		#pragma warning(disable: 4715)
	#elif defined(__GNUG__)
		#pragma GCC diagnostic ignored "-Wreturn-type"
	#elif defined(__clang__)
	#else
		// TODO suppress warnings for other compilers
	#endif
#endif

namespace stdx::flow // This probably will be unnecessary
{
/*	template <class Type>
	class _jmp_variable : public Type
	{
		template <class ... ArgTypes>
		_jmp_variable(ArgTypes && ... args) :
			Type(std::forward<ArgTypes>(args) ...)
		{
		}
	};

	template <class Type, class ... ArgTypes>
	decltype(auto) jmp_variable(ArgTypes && ... args)
	{
		static_assert(!std::is_final_v<Type>, "STDX_FLOW_VARIABLE(type, args ...): type must be a non-final class!");
		if constexpr (std::is_class_v<Type> && !std::is_trivially_destructible_v<Type>)
		{
			// TODO actually construct this in struct that controls its lifetime
			return _jmp_variable<Type>(std::forward<ArgTypes>(args) ...);
		}
		else
		{
			return Type(std::forward<ArgTypes>(args) ...);
		}
	} */

	// Executes invocation passed as argument while setting a point to return to without actually returning from that function
	#define STDX_FLOW_WAYPOINT_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_WAYPOINT_INVOKE, invocation)
	#define STDX_FLOW_WAYPOINT_INVOKE_FUNCTION(context, unique, invocation) \
	[&] () -> decltype(invocation)\
	{\
		/* TODO mark variables scope */\
		thread_local int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0;\
		struct STDX_MACRO_TYPE(scoped_thread, context, unique)\
		{\
			~STDX_MACRO_TYPE(scoped_thread, context, unique)()\
			{\
				/* TODO clean variables */\
				::stdx::this_thread::get_jmp_state().pop();\
				::stdx::this_thread::get_jmp_state().status() = STDX_MACRO_VARIABLE(status, context, unique);\
			}\
		}\
		volatile STDX_MACRO_VARIABLE(scoped_this_thread, context, unique);\
		if ((STDX_MACRO_VARIABLE(status, context, unique) = setjmp(::stdx::this_thread::get_jmp_state().emplace())) == 0)\
		{\
			return invocation;\
		}\
	}\
	()

	// TODO STDX_FLOW_INVOKE(invocation) 

	// Returns execution to last point in stack without returning from current function
	#define STDX_FLOW_RETURN(...) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_RETURN, __VA_ARGS__)
	#define STDX_FLOW_RETURN_FUNCTION(context, unique, ...) \
	[] (int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0)\
	{\
		if (!::stdx::this_thread::get_jmp_state().empty())\
		{\
			::std::longjmp(::stdx::this_thread::get_jmp_state().top(), STDX_MACRO_VARIABLE(status, context, unique));\
		}\
	}\
	(__VA_ARGS__)

	// Checks if previous invoke returned without jumping
	#define STDX_FLOW_CHECK_INVOKE() \
	[]\
	{\
		return ::stdx::this_thread::get_jmp_state().status() == 0;\
	}\
	()

	// Checks if next return will jump
	#define STDX_FLOW_CHECK_RETURN() \
	[]\
	{\
		return !::stdx::this_thread::get_jmp_state().empty();\
	}\
	()
	
	#define STDX_FLOW_VARIABLE(...) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_VARIABLE, __VA_ARGS__)
	#define STDX_FLOW_VARIABLE_FUNCTION(context, unique, type, ...) // TODO implementation
}

#endif
