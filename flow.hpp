#ifndef STDX_header_FLOW
#define STDX_header_FLOW

#include <csetjmp>
#include <memory>
#include <stack>

#include "core/stdx.hpp"

// Directive: suppress_warnings
#ifdef STDX_directive_FLOW_SUPPRESS_WARNINGS
	#if defined(__clang__)
		#pragma clang diagnostic ignored "-Wreturn-type" // TODO test this
	#elif defined(__GNUG__)
		#pragma GCC diagnostic ignored "-Wreturn-type"
	#elif defined(_MSC_VER)
		#pragma warning(disable: 4611) // MSVC implementation specific warning
//		#pragma warning(disable: 4715) // -Wreturn-type
	#else
		// TODO suppress warnings for other compilers
	#endif
#endif

namespace stdx::flow
{
	template <class Type>
	struct identity
	{
		using type = Type;
	};

	class _jmp_state
	{
		class _jmp_var_obj_base
		{
		public:
			virtual ~_jmp_var_obj_base() = default;

			virtual operator void *() = 0;

		};

		template <class Type>
		class _jmp_var_obj : _jmp_var_obj_base
		{
		public:
			~_jmp_var_obj() override
			{
				std::destroy_at(std::launder(reinterpret_cast<Type *>(&_obj)));
			}

			operator void *() override
			{
				return &_obj;
			}

		private:
			std::aligned_storage_t<sizeof(Type), alignof(Type)> _obj;

		};

		class _jmp_var
		{
		public:
			_jmp_var(std::unique_ptr<_jmp_var_obj_base> obj_ptr) :
				_obj_ptr(std::move(obj_ptr))
			{
			}

			operator void *() const
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
			std::jmp_buf _env{};

		};
	public:
		template <class Type>
		static void * push_var()
		{
			return _var_stack.top().top().emplace(std::make_unique<_jmp_var_obj<Type>>());
		}

		static void push_scope()
		{
			_var_stack.top().emplace();
		}

		static void pop_scope()
		{
			auto & stack = _var_stack.top().top();
			while (!stack.empty())
			{
				stack.pop();
			}
			_var_stack.top().pop();
		}

		static std::jmp_buf & push_buf()
		{
			_var_stack.emplace();
			push_scope();
			return _buf_stack.emplace();
		}

		static void pop_buf()
		{
			_buf_stack.pop();
			auto & stack = _var_stack.top();
			while (!stack.empty())
			{
				pop_scope();
			}
			_var_stack.pop();
		}

		static std::jmp_buf & get_buf()
		{
			return _buf_stack.top();
		}

		static int get_status()
		{
			return _status;
		}

		static void set_status(int status)
		{
			_status = status;
		}

	private:
		inline thread_local static std::stack<std::stack<std::stack<_jmp_var>>> _var_stack;
		inline thread_local static std::stack<_jmp_buf> _buf_stack;
		inline thread_local static int _status{ 0 };

	};
}

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it while setting a point to jump to without returning from the function
// The point is removed once the function returns or is jumped from
#define STDX_FLOW_SET_AND_INVOKE(invocation) STDX_MACRO_FUNCTION_N_ARY(FLOW_SET_AND_INVOKE, invocation)
#define STDX_implementation_FLOW_SET_AND_INVOKE(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(invocation)\
{\
	if (setjmp(::stdx::flow::_jmp_state::push_buf()) == 0)/*TODO we need to do some voodoo shit for lvalue references*/\
	{\
		if constexpr (::std::negation_v<::std::is_same<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>>)\
		{\
			decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
			::stdx::flow::_jmp_state::set_status(0);\
			::stdx::flow::_jmp_state::pop_buf();\
			return STDX_MACRO_VARIABLE(result, context);\
		}\
		else\
		{\
			invocation;\
			::stdx::flow::_jmp_state::set_status(0);\
			::stdx::flow::_jmp_state::pop_buf();\
		}\
	}\
	else\
	{\
	}\
}\
(::stdx::flow::identity<decltype(invocation)>())

// Returns execution to last set jump point
#define STDX_FLOW_JUMP(...) STDX_MACRO_FUNCTION_N_ARY(FLOW_JUMP, __VA_ARGS__)
#define STDX_implementation_FLOW_JUMP(context, ...) \
[] (int STDX_MACRO_VARIABLE(status, context) = 1)\
{\
	::stdx::flow::_jmp_state::set_status(STDX_MACRO_VARIABLE(status, context));\
	::std::longjmp(::stdx::flow::_jmp_state::get_buf(), 1);\
}\
(__VA_ARGS__)

// Checks the status of the last set jump point
#define STDX_FLOW_STATUS() STDX_MACRO_FUNCTION_0_ARY(FLOW_STATUS)
#define STDX_implementation_FLOW_STATUS(context) \
[]\
{\
	return ::stdx::flow::_jmp_state::get_status();\
}\
()

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it
#define STDX_FLOW_INVOKE(invocation) STDX_MACRO_FUNCTION_N_ARY(FLOW_INVOKE, invocation)
#define STDX_implementation_FLOW_INVOKE(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_result, context)) -> decltype(invocation)\
{\
	::stdx::flow::_jmp_state::push_scope();\
	if constexpr (::std::negation_v<::std::is_same<void, typename decltype(STDX_MACRO_VARIABLE(invocation_result, context))::type>>)\
	{\
		decltype(auto) STDX_MACRO_VARIABLE(result, context) = invocation;\
		::stdx::flow::_jmp_state::pop_scope();\
		return STDX_MACRO_VARIABLE(result, context);\
	}\
	else\
	{\
		invocation;\
		::stdx::flow::_jmp_state::pop_scope();\
	}\
}\
(::stdx::flow::identity<decltype(invocation)>())

// Declares a jump-protected variable, whose lifetime is controlled by the containing jump-protected scope
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_N_ARY(FLOW_DECLARE, declaration)
#define STDX_implementation_FLOW_DECLARE(context, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_result, context)) -> decltype(auto)\
{\
	if constexpr (::std::negation_v<::std::is_trivially_destructible<decltype(STDX_MACRO_VARIABLE(declaration_result, context))::type>>)\
	{\
		return *new (::stdx::flow::_jmp_state::push_var<decltype(declaration)>()) declaration;\
	}\
	else\
	{\
		return declaration;\
	}\
}\
(::stdx::flow::identity<decltype(declaration)>())

// Introduces a jump-protected scope on any jump-protected variables declared in the next statement
// Continue and break statements are allowed in the next statement: both cause the execution flow to exit the scope and move to the next instruction
#define STDX_FLOW_SCOPE STDX_MACRO_FUNCTION_0_ARY(FLOW_SCOPE)
#define STDX_implementation_FLOW_SCOPE(context) \
for (\
	bool STDX_MACRO_VARIABLE(scope, context) = true;\
	[&STDX_MACRO_VARIABLE(scope, context)]\
	{\
		if (STDX_MACRO_VARIABLE(scope, context))\
		{\
			::stdx::flow::_jmp_state::push_scope();\
		}\
		else\
		{\
			::stdx::flow::_jmp_state::pop_scope();\
		}\
		return STDX_MACRO_VARIABLE(scope, context);\
	}\
	();\
	STDX_MACRO_VARIABLE(scope, context) = false\
)\
switch(0)\
default:

#endif
