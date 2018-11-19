#ifndef STDX_FLOW_HPP
#define STDX_FLOW_HPP

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

// Executes invocation while defining a point to jump to without returning from the current call stack
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_INVOKE, invocation)
#define STDX_FLOW_INVOKE_FUNCTION(context, uid, invocation) \
[&] () -> decltype(invocation)\
{\
	struct STDX_MACRO_TYPE(scoped_thread, context, uid)\
	{\
		~STDX_MACRO_TYPE(scoped_thread, context, uid)()\
		{\
			while (::stdx::this_thread::get_jmp_state().check_var())\
			{\
				::stdx::this_thread::get_jmp_state().pop_var();\
			}\
			::stdx::this_thread::get_jmp_state().pop_stack();\
			::stdx::this_thread::get_jmp_state().pop_env();\
		}\
	}\
	volatile STDX_MACRO_VARIABLE(scoped_this_thread, context, uid);\
	if (setjmp(::stdx::this_thread::get_jmp_state().push_env()) == 0)\
	{\
		::stdx::this_thread::get_jmp_state().push_stack();\
		::stdx::this_thread::get_jmp_state().set_status(0);\
		return invocation;\
	}\
}\
()

// Returns execution to last defined jump point without returning from current call stack
#define STDX_FLOW_RETURN(...) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_RETURN, __VA_ARGS__)
#define STDX_FLOW_RETURN_FUNCTION(context, uid, ...) \
[] (int STDX_MACRO_VARIABLE(status, context, uid) = 1)\
{\
	if (::stdx::this_thread::get_jmp_state().check_env())\
	{\
		::stdx::this_thread::get_jmp_state().set_status(STDX_MACRO_VARIABLE(status, context, uid));\
		::std::longjmp(::stdx::this_thread::get_jmp_state().get_env(), STDX_MACRO_VARIABLE(status, context, uid));\
	}\
}\
(__VA_ARGS__)

// Checks if the previous invoke returned without jumping
#define STDX_FLOW_CHECK_INVOKE() \
[]\
{\
	return ::stdx::this_thread::get_jmp_state().get_status() == 0;\
}\
()

// Checks if the next return will jump
#define STDX_FLOW_CHECK_RETURN() \
[]\
{\
	return ::stdx::this_thread::get_jmp_state().check_env();\
}\
()

// TODO cases for class, array and function types
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_n_ary(STDX_FLOW_DECLARE, declaration)
#define STDX_FLOW_DECLARE_FUNCTION(context, uid, declaration) \
[&] () -> decltype(declaration)\
{\
	if constexpr (std::is_trivially_destructible_v<decltype(declaration)>)\
	{\
		return declaration;\
	}\
	else\
	{\
		return *new (stdx::this_thread::get_jmp_state().push_var<decltype(declaration)>()) declaration;/*Placement new, push_var should return void pointer*/\
	}\
}\
()

#define STDX_FLOW_SCOPE() \
while (\
[]\
{\
	thread_local bool within = false;\
	if ((within = !within) == true)\
	{\
		/*TODO Mark new scope*/\
	}\
	else\
	{\
		/*TODO Delete variables*/\
	}\
	return within;\
}\
()\
)

/*
namespace stdx::flow
{
	template <class RetType, class ... ParamTypes>
	RetType invoke(RetType(* func)(ParamTypes ...), ParamTypes && ... args)
	{
		stdx::this_thread::get_jmp_state().push_stack();

		RetType result = func(std::forward<ParamTypes>(args) ...);

		while (stdx::this_thread::get_jmp_state().check_var())
		{
			stdx::this_thread::get_jmp_state().pop_var();
		}
		stdx::this_thread::get_jmp_state().pop_stack();

		return result;
	}
	template <class ... ParamTypes>
	void invoke(void(* func)(ParamTypes ...), ParamTypes && ... args)
	{
		stdx::this_thread::get_jmp_state().push_stack();

		func(std::forward<ParamTypes>(args) ...);

		while (stdx::this_thread::get_jmp_state().check_var())
		{
			stdx::this_thread::get_jmp_state().pop_var();
		}
		stdx::this_thread::get_jmp_state().pop_stack();
	}
	template <class RetType, class ObjType, class ... ParamTypes>
	RetType invoke(RetType(ObjType::* func)(ParamTypes ...), ObjType & obj, ParamTypes && ... args)
	{
		stdx::this_thread::get_jmp_state().push_stack();

		RetType result = obj.*func(std::forward<ParamTypes>(args) ...);

		while (stdx::this_thread::get_jmp_state().check_var())
		{
			stdx::this_thread::get_jmp_state().pop_var();
		}
		stdx::this_thread::get_jmp_state().pop_stack();

		return result;
	}
	template <class ObjType, class ... ParamTypes>
	void invoke(void(ObjType::* func)(ParamTypes ...), ObjType & obj, ParamTypes && ... args)
	{
		stdx::this_thread::get_jmp_state().push_stack();

		obj.*func(std::forward<ParamTypes>(args) ...);

		while (stdx::this_thread::get_jmp_state().check_var())
		{
			stdx::this_thread::get_jmp_state().pop_var();
		}
		stdx::this_thread::get_jmp_state().pop_stack();
	}

	template <class Type, class ... ArgTypes>
	decltype(auto) declare(ArgTypes && ... args)
	{
		if constexpr (std::is_trivially_destructible_v<Type>)
		{
			return Type(std::forward<ArgTypes>(args) ...);
		}
		else
		{
			return stdx::this_thread::get_jmp_state().push_var<Type>(std::forward<ArgTypes>(args) ...);
		}
	}
}
*/

#endif
