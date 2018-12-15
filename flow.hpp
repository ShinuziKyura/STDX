#ifndef STDX_implementation_FLOW_HEADER
#define STDX_implementation_FLOW_HEADER

#include <csetjmp>
#include <memory>
#include <stack>

#include "stdx/core.hpp"

// Directive: suppress_warnings
#ifdef STDX_directive_FLOW_SUPPRESS_WARNINGS
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
	
namespace stdx::flow
{
	class _jmp_state
	{
		struct _jmp_var_obj_base
		{
			virtual ~_jmp_var_obj_base() = default;

			virtual operator void *() = 0;
		};
		template <class Type>
		struct _jmp_var_obj : _jmp_var_obj_base
		{
			~_jmp_var_obj() override
			{
				std::destroy_at(std::launder(reinterpret_cast<Type *>(&_obj)));
			}

			operator void *() override
			{
				return &_obj;
			}

			std::aligned_storage_t<sizeof(Type), alignof(Type)> _obj;
		};

		class _jmp_var
		{
		public:
			template <class Type>
			_jmp_var(std::common_type<Type>) :
				_obj_ptr(std::make_unique<_jmp_var_obj<Type>>())
			{
			}

			operator void *()
			{
				return *_obj_ptr;
			}
		private:
			std::unique_ptr<_jmp_var_obj_base> _obj_ptr;
		};
		class _jmp_buf
		{
		public:
			operator std::jmp_buf &()
			{
				return _env;
			}
		private:
			std::jmp_buf _env;
		};

		using _jmp_var_stack = std::stack<std::stack<_jmp_var>>;
	public:
		template <class Type>
		void * push_var()
		{
			return _var_stack.top().top().emplace(std::common_type<Type>());
		}

		void push_stack()
		{
			_var_stack.top().emplace();
		}
		void pop_stack()
		{
			auto & stack = _var_stack.top().top();
			while (!stack.empty())
			{
				stack.pop();
			}
			_var_stack.top().pop();
		}

		std::jmp_buf & push_buf()
		{
			_var_stack.emplace();
			push_stack();
			return _buf_stack.emplace();
		}
		void pop_buf()
		{
			auto & stack = _var_stack.top();
			while (!stack.empty())
			{
				pop_stack();
			}
			_var_stack.pop();
			_buf_stack.pop();
		}
		std::jmp_buf & get_buf()
		{
			return _buf_stack.top();
		}

		int get_status()
		{
			return _status;
		}
		void set_status(int status)
		{
			_status = status;
		}
	private:
		std::stack<_jmp_var_stack> _var_stack;
		std::stack<_jmp_buf> _buf_stack;
		int _status{ 0 };
	};

	inline _jmp_state & jmp_state()
	{
		thread_local static _jmp_state state;
		return state;
	}
}

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it while setting a point to jump to without returning from the function
// The point is removed once the function returns or is jumped from
#define STDX_FLOW_SET_AND_INVOKE(invocation) STDX_MACRO_FUNCTION_N_ARY(FLOW_SET_AND_INVOKE, invocation)
#define STDX_implementation_FLOW_SET_AND_INVOKE(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(auto)\
{\
	if (setjmp(::stdx::flow::jmp_state().push_buf()) == 0)\
	{\
		if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>)\
		{\
			decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
			::stdx::flow::jmp_state().set_status(0);\
			::stdx::flow::jmp_state().pop_buf();\
			return STDX_MACRO_VARIABLE(result, context);\
		}\
		else\
		{\
			invocation;\
			::stdx::flow::jmp_state().set_status(0);\
		}\
	}\
	::stdx::flow::jmp_state().pop_buf();\
}\
(::std::common_type<decltype(invocation)>())

// Returns execution to last set jump point
#define STDX_FLOW_JUMP(...) STDX_MACRO_FUNCTION_N_ARY(FLOW_JUMP, __VA_ARGS__)
#define STDX_implementation_FLOW_JUMP(context, ...) \
[] (int STDX_MACRO_VARIABLE(status, context) = 1)\
{\
	::stdx::flow::jmp_state().set_status(STDX_MACRO_VARIABLE(status, context));\
	::std::longjmp(::stdx::flow::jmp_state().get_buf(), 1);\
}\
(__VA_ARGS__)

// Checks the status of the last set jump point
#define STDX_FLOW_STATUS() STDX_MACRO_FUNCTION_0_ARY(FLOW_STATUS)
#define STDX_implementation_FLOW_STATUS(context) \
[]\
{\
	return ::stdx::flow::jmp_state().get_status();\
}\
()

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_N_ARY(FLOW_INVOKE, invocation)
#define STDX_implementation_FLOW_INVOKE(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(auto)\
{\
	::stdx::flow::jmp_state().push_stack();\
	if constexpr (!::std::is_same_v<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>)\
	{\
		decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
		::stdx::flow::jmp_state().pop_stack();\
		return STDX_MACRO_VARIABLE(result, context);\
	}\
	else\
	{\
		invocation;\
	}\
	::stdx::flow::jmp_state().pop_stack();\
}\
(::std::common_type<decltype(invocation)>())

// Declares a jump-protected variable, whose lifetime is controlled by the containing jump-protected scope
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_N_ARY(FLOW_DECLARE, declaration)
#define STDX_implementation_FLOW_DECLARE(context, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_result, context)) -> decltype(auto)\
{\
	if constexpr (!::std::is_trivially_destructible_v<typename decltype(STDX_MACRO_VARIABLE(declaration_result, context))::type>)\
	{\
		return *new (::stdx::flow::jmp_state().push_var<decltype(declaration)>()) declaration;\
	}\
	else\
	{\
		return declaration;\
	}\
}\
(::std::common_type<decltype(declaration)>())

// Introduces a jump-protected scope on any jump-protected variables declared in the next statement
// [Note: This statement should not be terminated by a semicolon --end note]
#define STDX_FLOW_SCOPE STDX_MACRO_FUNCTION_0_ARY(FLOW_SCOPE)
#define STDX_implementation_FLOW_SCOPE(context) \
for (\
	bool STDX_MACRO_VARIABLE(within, context) = true;\
	[&STDX_MACRO_VARIABLE(within, context)]\
	{\
		if (STDX_MACRO_VARIABLE(within, context))\
		{\
			::stdx::flow::jmp_state().push_stack();\
		}\
		else\
		{\
			::stdx::flow::jmp_state().pop_stack();\
		}\
		return STDX_MACRO_VARIABLE(within, context);\
	}\
	();\
	STDX_MACRO_VARIABLE(within, context) = false\
)

#endif
