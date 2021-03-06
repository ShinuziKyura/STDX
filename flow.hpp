#ifndef STDX_header_FLOW
#define STDX_header_FLOW

#include <csetjmp>
#include <type_traits>
#include <memory>
#include <new>
#include <stack>

#include "core/stdx.hpp"

#ifdef _MSC_VER
	#pragma warning(disable: 4611) // MSVC implementation specific warning
#endif

namespace stdx::flow
{
	template <class Type>
	struct _type_identity
	{
		using type = Type;
	};

	class _jmp_state
	{
	public:
		template <class Type>
		static void * push_var()
		{
			return _jmp_stack.top()._var_scope_stack.top().emplace(std::make_unique<_jmp_var_obj<Type>>());
		}

		static void push_scope()
		{
			_jmp_stack.top()._var_scope_stack.emplace();
		}

		static void pop_scope()
		{
			auto & var_stack = _jmp_stack.top()._var_scope_stack.top();
			while (!var_stack.empty())
			{
				var_stack.pop();
			}
			_jmp_stack.top()._var_scope_stack.pop();
		}

		static std::jmp_buf & push_buf()
		{
			_jmp_stack.emplace();
			push_scope();
			return _jmp_stack.top()._buf;
		}

		static void pop_buf()
		{
			auto & var_stack = _jmp_stack.top()._var_scope_stack;
			while (!var_stack.empty())
			{
				pop_scope();
			}
			_jmp_stack.pop();
		}

		static std::jmp_buf & get_buf()
		{
			return _jmp_stack.top()._buf;
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
		class _jmp_var_obj_base
		{
		public:
			virtual ~_jmp_var_obj_base() = default;

			virtual operator void *() = 0;

		};

		template <class Type>
		class alignas(Type) _jmp_var_obj : public _jmp_var_obj_base
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

		class _jmp_point
		{
		public:
			std::stack<std::stack<_jmp_var>> _var_scope_stack;
			_jmp_buf _buf;

		};

		inline thread_local static std::stack<_jmp_point> _jmp_stack;
		inline thread_local static int _status{ 0 };

	};
}

// Invokes a function with a jump-protected scope on any jump-protected variables declared in it while setting a point to jump to without returning from the function
// The point is removed once the function returns or is jumped from
#define STDX_FLOW_SET_AND_INVOKE(invocation) STDX_MACRO_FUNCTION_N_ARY(FLOW_SET_AND_INVOKE, invocation)
#define STDX_implementation_FLOW_SET_AND_INVOKE(context, invocation) \
[&] (auto STDX_MACRO_VARIABLE(invocation_type, context)) -> decltype(invocation)\
{\
	using invocation_type = decltype(STDX_MACRO_VARIABLE(invocation_type, context))::type;\
	struct STDX_MACRO_TYPE(scope_guard, context)\
	{\
		~STDX_MACRO_TYPE(scope_guard, context)()\
		{\
			::stdx::flow::_jmp_state::pop_buf();\
		}\
	}\
	STDX_MACRO_VARIABLE(scope_guard, context);\
	::stdx::flow::_jmp_state::set_status(0);\
	auto & STDX_MACRO_VARIABLE(buf, context) = ::stdx::flow::_jmp_state::push_buf();\
	if (setjmp(STDX_MACRO_VARIABLE(buf, context)))\
	{\
		if constexpr (::std::is_lvalue_reference_v<invocation_type> && !::std::is_const_v<::std::remove_reference_t<invocation_type>>)\
		{\
			::std::remove_reference_t<invocation_type> STDX_MACRO_VARIABLE(lvalue, context);\
			return STDX_MACRO_VARIABLE(lvalue, context);\
		}\
		else\
		{\
			return ::std::remove_reference_t<invocation_type>();\
		}\
	}\
	return invocation;\
}\
(::stdx::flow::_type_identity<decltype(invocation)>())

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
[&] () -> decltype(invocation)\
{\
	struct STDX_MACRO_TYPE(scope_guard, context)\
	{\
		~STDX_MACRO_TYPE(scope_guard, context)()\
		{\
			::stdx::flow::_jmp_state::pop_scope();\
		}\
	}\
	STDX_MACRO_VARIABLE(scope_guard, context);\
	::stdx::flow::_jmp_state::push_scope();\
	return invocation;\
}\
()

// Declares a jump-protected variable, whose lifetime is controlled by the containing jump-protected scope
#define STDX_FLOW_DECLARE(declaration) STDX_MACRO_FUNCTION_N_ARY(FLOW_DECLARE, declaration)
#define STDX_implementation_FLOW_DECLARE(context, declaration) \
[&] (auto STDX_MACRO_VARIABLE(declaration_type, context)) -> decltype(declaration)\
{\
	using declaration_type = decltype(STDX_MACRO_VARIABLE(declaration_type, context))::type;\
	if constexpr (!::std::is_trivially_destructible_v<::std::decay_t<declaration_type>>)\
	{\
		return *new (::stdx::flow::_jmp_state::push_var<declaration_type>()) declaration;\
	}\
	else\
	{\
		return declaration;\
	}\
}\
(::stdx::flow::_type_identity<decltype(declaration)>())

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
