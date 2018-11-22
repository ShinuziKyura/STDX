#ifndef STDX_FLOW_HPP
#define STDX_FLOW_HPP

#include "meta.hpp"
#include "thread.hpp"
#include "stdx_macros.hpp"

#ifdef STDX_FLOW_SUPPRESS_WARNINGS
	#if defined(_MSVC_LANG)
		#pragma warning(disable: 4611)
		#pragma warning(disable: 4715)
	#elif defined(__GNUG__)
		#pragma GCC diagnostic ignored "-Wreturn-type"
	#elif defined(__clang__)
		//#pragma clang diagnostic ignored "-Wreturn-type" // TODO test this
	#else
		// TODO suppress warnings for other compilers
	#endif
#endif

// Executes invocation while defining a point to jump to without returning through the call stack
#define STDX_FLOW_JUMP_POINT(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_JUMP_POINT, invocation)
#define STDX_FLOW_JUMP_POINT_FUNCTION(context, uid, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context, uid)) -> decltype(invocation)\
{\
	if (setjmp(::stdx::this_thread::jmp_state().push_env()) == 0)\
	{\
		if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context, uid))::type>)\
		{\
			decltype(auto) STDX_MACRO_VARIABLE(result, context, uid) = invocation;\
			::stdx::this_thread::jmp_state().set_status(0);\
			::stdx::this_thread::jmp_state().pop_env();\
			return STDX_MACRO_VARIABLE(result, context, uid);\
		}\
		else\
		{\
			invocation;\
			::stdx::this_thread::jmp_state().set_status(0);\
		}\
	}\
	::stdx::this_thread::jmp_state().pop_env();\
}\
(::stdx::meta::identity<decltype(invocation)>())

// Checks if there is a valid jump point
#define STDX_FLOW_JUMP_POINT_STATUS() \
[]\
{\
	return ::stdx::this_thread::jmp_state().check_env();\
}\
()

// Returns execution to last defined jump point without returning through the call stack
#define STDX_FLOW_JUMP(...) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_JUMP, __VA_ARGS__)
#define STDX_FLOW_JUMP_FUNCTION(context, uid, ...) \
[] (int STDX_MACRO_VARIABLE(status, context, uid) = 1)\
{\
	if (::stdx::this_thread::jmp_state().check_env())\
	{\
		::stdx::this_thread::jmp_state().set_status(STDX_MACRO_VARIABLE(status, context, uid));\
		::std::longjmp(::stdx::this_thread::jmp_state().get_env(), STDX_MACRO_VARIABLE(status, context, uid));\
	}\
}\
(__VA_ARGS__)

// Checks if the last jump point returned without jumping
#define STDX_FLOW_JUMP_STATUS() \
[]\
{\
	return ::stdx::this_thread::jmp_state().get_status() == 0;\
}\
()

// TODO description
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_INVOKE, invocation)
#define STDX_FLOW_INVOKE_FUNCTION(context, uid, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context, uid)) -> decltype(invocation)\
{\
	::stdx::this_thread::jmp_state().push_stack();\
	if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context, uid))::type>)\
	{\
		decltype(auto) STDX_MACRO_VARIABLE(result, context, uid) = invocation;\
		::stdx::this_thread::jmp_state().pop_stack();\
		return STDX_MACRO_VARIABLE(result, context, uid);\
	}\
	else\
	{\
		invocation;\
	}\
	::stdx::this_thread::jmp_state().pop_stack();\
}\
(::stdx::meta::identity<decltype(invocation)>())

// TODO description
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_DECLARE, declaration) // TODO cases for class, array and function types
#define STDX_FLOW_DECLARE_FUNCTION(context, uid, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_result, context, uid)) -> decltype(declaration)\
{\
	if constexpr (::std::is_trivially_destructible_v<typename decltype(STDX_MACRO_VARIABLE(declaration_result, context, uid))::type>)\
	{\
		return declaration;\
	}\
	else\
	{\
		return *new (::stdx::this_thread::jmp_state().push_var<decltype(declaration)>()) declaration;\
	}\
}\
(::stdx::meta::identity<decltype(declaration)>())

// TODO description
#define STDX_FLOW_SCOPE() \
while (\
[]\
{\
	thread_local bool within = false;\
	if ((within = !within) == true)\
	{\
		::stdx::this_thread::jmp_state().push_stack();\
	}\
	else\
	{\
		::stdx::this_thread::jmp_state().pop_stack();\
	}\
	return within;\
}\
()\
)

#endif
