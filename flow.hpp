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

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it while setting a point to jump to without returning from the function
// The point is removed once the function returns or is jumped from
#define STDX_FLOW_SET_AND_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_SET_AND_INVOKE, invocation)
#define STDX_FLOW_SET_AND_INVOKE_function_(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(auto)\
{\
	if (setjmp(::stdx::this_thread::jmp_state().push_buf()) == 0)\
	{\
		if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>)\
		{\
			decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
			::stdx::this_thread::jmp_state().set_status(0);\
			::stdx::this_thread::jmp_state().pop_buf();\
			return STDX_MACRO_VARIABLE(result, context);\
		}\
		else\
		{\
			invocation;\
			::stdx::this_thread::jmp_state().set_status(0);\
		}\
	}\
	::stdx::this_thread::jmp_state().pop_buf();\
}\
(::std::common_type<decltype(invocation)>())

// Returns execution to last set jump point
#define STDX_FLOW_JUMP(...) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_JUMP, __VA_ARGS__)
#define STDX_FLOW_JUMP_function_(context, ...) \
[] (int STDX_MACRO_VARIABLE(status, context) = 1)\
{\
	::stdx::this_thread::jmp_state().set_status(STDX_MACRO_VARIABLE(status, context));\
	::std::longjmp(::stdx::this_thread::jmp_state().get_buf(), 1);\
}\
(__VA_ARGS__)

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_INVOKE, invocation)
#define STDX_FLOW_INVOKE_function_(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(auto)\
{\
	::stdx::this_thread::jmp_state().push_stack();\
	if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>)\
	{\
		decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
		::stdx::this_thread::jmp_state().pop_stack();\
		return STDX_MACRO_VARIABLE(result, context);\
	}\
	else\
	{\
		invocation;\
	}\
	::stdx::this_thread::jmp_state().pop_stack();\
}\
(::std::common_type<decltype(invocation)>())

// Declares a jump-protected variable, whose lifetime is controlled by the containing jump-protected scope
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_DECLARE, declaration)
#define STDX_FLOW_DECLARE_function_(context, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_result, context)) -> decltype(auto)\
{\
	if constexpr (!::std::is_trivially_destructible_v<typename decltype(STDX_MACRO_VARIABLE(declaration_result, context))::type>)\
	{\
		return *new (::stdx::this_thread::jmp_state().push_var<decltype(declaration)>()) declaration;\
	}\
	else\
	{\
		return declaration;\
	}\
}\
(::std::common_type<decltype(declaration)>())

// Introduces a jump-protected scope on any jump-protected variables declared in the statement that follows this one 
// [Note: This statement should not be terminated by a semicolon --end note]
#define STDX_FLOW_SCOPE() STDX_MACRO_FUNCTION_0_ary(STDX_FLOW_SCOPE)
#define STDX_FLOW_SCOPE_function_(context) \
for (\
	bool STDX_MACRO_VARIABLE(within, context) = true;\
	[&STDX_MACRO_VARIABLE(within, context)]\
	{\
		if (STDX_MACRO_VARIABLE(within, context))\
		{\
			::stdx::this_thread::jmp_state().push_stack();\
		}\
		else\
		{\
			::stdx::this_thread::jmp_state().pop_stack();\
		}\
		return STDX_MACRO_VARIABLE(within, context);\
	}\
	();\
	STDX_MACRO_VARIABLE(within, context) = false\
)

// Checks if the last invocation returned without jumping
#define STDX_FLOW_STATUS() \
[]\
{\
	return ::stdx::this_thread::jmp_state().get_status();\
}\
()

#endif
