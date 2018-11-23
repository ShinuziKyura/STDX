#ifndef STDX_FLOW_HPP
#define STDX_FLOW_HPP

#include "thread.hpp"
#include "stdx_macros.hpp"

#ifdef STDX_FLOW_SUPPRESS_WARNINGS
	#if defined(__clang__)
		#pragma clang diagnostic ignored "-Wreturn-type" // TODO test this
	#elif defined(__GNUG__)
		#pragma GCC diagnostic ignored "-Wreturn-type"
	#elif defined(_MSC_VER)
		#pragma warning(disable: 4611)
		#pragma warning(disable: 4715)
	#else
		// TODO suppress warnings for other compilers
	#endif
#endif

// Executes invocation while marking a point to jump to without returning from the invocation
// The point is removed once the invocation returns or jumps
#define STDX_FLOW_MARK_AND_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_MARK_AND_INVOKE, invocation)
#define STDX_FLOW_MARK_AND_INVOKE_function_(context, uid, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context, uid)) -> decltype(auto)\
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
(::std::common_type<decltype(invocation)>())

// Returns execution to last marked jump point without returning through the call stack
#define STDX_FLOW_JUMP() \
[]\
{\
	::stdx::this_thread::jmp_state().set_status(1);\
	::std::longjmp(::stdx::this_thread::jmp_state().get_env(), 1);\
}\
()

// Invokes a function with a jump-protected scope on any jump-protected variables declared in its definition
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_INVOKE, invocation)
#define STDX_FLOW_INVOKE_function_(context, uid, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context, uid)) -> decltype(auto)\
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
(::std::common_type<decltype(invocation)>())

// Declares a jump-protected variable, limited by the scope of the outer STDX_FLOW_JUMP_POINT / STDX_FLOW_INVOKE / STDX_FLOW_SCOPE
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_DECLARE, declaration)
#define STDX_FLOW_DECLARE_function_(context, uid, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_result, context, uid)) -> decltype(auto)\
{\
	static_assert(!::std::is_array_v<typename decltype(STDX_MACRO_VARIABLE(declaration_result, context, uid))::type>,\
				  "'STDX_FLOW_DECLARE(declaration)': declaration must not be an array type");\
	if constexpr (::std::is_trivially_destructible_v<typename decltype(STDX_MACRO_VARIABLE(declaration_result, context, uid))::type>)\
	{\
		return declaration;\
	}\
	else\
	{\
		return *new (::stdx::this_thread::jmp_state().push_var<decltype(declaration)>()) declaration;\
	}\
}\
(::std::common_type<decltype(declaration)>())

// Introduces a jump-protected scope on any jump-protected variables declared in the statement that follows this one 
// [Note: This statement should not be terminated by a semicolon -- end note]
#define STDX_FLOW_SCOPE() STDX_MACRO_FUNCTION_0_ary(STDX_FLOW_SCOPE)
#define STDX_FLOW_SCOPE_function_(context, uid) \
for (bool STDX_MACRO_VARIABLE(within, context, uid) = true;\
[&STDX_MACRO_VARIABLE(within, context, uid)]\
{\
	if (STDX_MACRO_VARIABLE(within, context, uid))\
	{\
		::stdx::this_thread::jmp_state().push_stack();\
	}\
	else\
	{\
		::stdx::this_thread::jmp_state().pop_stack();\
	}\
	return STDX_MACRO_VARIABLE(within, context, uid);\
}\
();\
STDX_MACRO_VARIABLE(within, context, uid) = false)

// Checks if the last invocation returned without jumping
#define STDX_FLOW_STATUS() \
[]\
{\
	return ::stdx::this_thread::jmp_state().get_status() == 0;\
}\
()

#endif
