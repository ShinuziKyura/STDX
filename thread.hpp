#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>
#include <type_traits>
#include <thread>
#include <mutex>
#include <stack>
#include <unordered_map>

#include "utility.hpp"

#ifdef STDX_THREAD_SUPPRESS_WARNINGS
#if defined(_MSC_VER)
#pragma warning(disable: 4611)
#pragma warning(disable: 4715)
#else
// TODO suppress warnings for other compilers
#endif
#endif

// IDEA see https://stackoverflow.com/questions/18542804/global-static-initialization-threading

// Checks if previous invoke returned without jumping
#define STDX_THREAD_JUMP_CHECK_INVOKE() \
[]\
{\
	return !::stdx::thread::_ttable::access(::std::this_thread::get_id())._status;\
}\
()

// Checks if next return will jump
#define STDX_THREAD_JUMP_CHECK_RETURN() \
[]\
{\
	return !::stdx::thread::_ttable::access(::std::this_thread::get_id())._context.empty();\
}\
()

#define STDX_THREAD_JUMP_INVOKE_(context, unique, ...) \
[&] () -> decltype(__VA_ARGS__)\
{\
	auto & STDX_MACRO_VARIABLE(this_thread, context, unique) = ::stdx::thread::_ttable::access(::std::this_thread::get_id());\
	struct STDX_MACRO_TYPE(scoped_thread, context, unique)\
	{\
		decltype(STDX_MACRO_VARIABLE(this_thread, context, unique)) STDX_MACRO_VARIABLE(_this_thread, context, unique);\
		STDX_MACRO_TYPE(scoped_thread, context, unique)(decltype(STDX_MACRO_VARIABLE(this_thread, context, unique)) STDX_MACRO_VARIABLE(thread, context, unique)) :\
			STDX_MACRO_VARIABLE(_this_thread, context, unique)(STDX_MACRO_VARIABLE(thread, context, unique))\
		{\
		}\
		~STDX_MACRO_TYPE(scoped_thread, context, unique)()\
		{\
			STDX_MACRO_VARIABLE(_this_thread, context, unique)._context.pop();\
		}\
	};\
	STDX_MACRO_TYPE(scoped_thread, context, unique) volatile STDX_MACRO_VARIABLE(scoped_this_thread, context, unique)(STDX_MACRO_VARIABLE(this_thread, context, unique));\
	switch (int volatile STDX_MACRO_VARIABLE(status, context, unique) = setjmp(STDX_MACRO_VARIABLE(this_thread, context, unique)._context.emplace()))\
	{\
		case 0:\
		{\
			STDX_MACRO_VARIABLE(this_thread, context, unique)._status = 0;\
			return __VA_ARGS__;\
		}\
		default:\
		{\
			STDX_MACRO_VARIABLE(this_thread, context, unique)._status = STDX_MACRO_VARIABLE(status, context, unique);\
		}\
	}\
}\
()
// Executes invocation passed as argument while setting a point to return to without actually returning from that function
#define STDX_THREAD_JUMP_INVOKE(...) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JUMP_INVOKE, __VA_ARGS__)

#define STDX_THREAD_JUMP_RETURN_(context, unique, ...) \
[] (int volatile STDX_MACRO_VARIABLE(status, context, unique) = 0)\
{\
	auto & STDX_MACRO_VARIABLE(this_thread, context, unique) = ::stdx::thread::_ttable::access(::std::this_thread::get_id());\
	if (!STDX_MACRO_VARIABLE(this_thread, context, unique)._context.empty())\
	{\
		std::longjmp(STDX_MACRO_VARIABLE(this_thread, context, unique)._context.top(), STDX_MACRO_VARIABLE(status, context, unique));\
	}\
}\
(__VA_ARGS__)
// Returns execution to last point in stack without returning from current function
#define STDX_THREAD_JUMP_RETURN(...) STDX_MACRO_FUNCTION_n_ary(STDX_THREAD_JUMP_RETURN, __VA_ARGS__)

namespace stdx
{
	struct this_thread
	{
		thread_local static inline std::stack<std::jmp_buf> jump_buffer; // jump_execution_context_stack
		thread_local static inline int jump_status = 0;
	};
}

namespace stdx::thread
{
	class _ttable // TODO make this into a thread local variable with just the needed defaults
	{
		struct _tjump //_thread_state
		{
			std::stack<std::jmp_buf> _context;
			int _status = 0;
		};
	public:
		static _tjump & access(std::thread::id const & id)
		{
			if (id != std::this_thread::get_id())
			{
				throw std::exception(); // TODO proper exception
			}

			STDX_SCOPED_VARIABLE(std::scoped_lock<std::mutex>(_mutex)); // TODO make this more dynamic, only lock if insertion is needed (read-write lock with promotion from read to write)

			return _table[id];
		}
	private:
		static inline std::unordered_map<std::thread::id, _tjump> _table;
		static inline std::mutex _mutex;
	};


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
