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
#endif
#endif

// Checks if previous invoke returned without jumping
#define STDX_THREAD_JUMP_CHECK_INVOKE() \
[]\
{\
	return !::stdx::thread::_ttable::access(std::this_thread::get_id())._status;\
}\
()

// Checks if next return will jump
#define STDX_THREAD_JUMP_CHECK_RETURN() \
[]\
{\
	return !::stdx::thread::_ttable::access(std::this_thread::get_id())._stack.empty();\
}\
()

// Executes invocation passed as argument while setting a point to return to without actually returning from that function
#define STDX_THREAD_JUMP_INVOKE(...) \
[&] () -> decltype(__VA_ARGS__)\
{\
	auto & thread = ::stdx::thread::_ttable::access(std::this_thread::get_id());\
	switch (int status = setjmp(thread._stack.emplace()))\
	{\
		case 0:\
		{\
			thread._status = 0;\
			auto && ret = __VA_ARGS__;\
			thread._stack.pop();\
			return ret;\
		}\
		default:\
		{\
			thread._status = status;\
			thread._stack.pop();\
		}\
	}\
}\
()

// Returns execution to last point in stack without returning from current function
#define STDX_THREAD_JUMP_RETURN(...) \
[] (int status = 0)\
{\
	auto & thread = ::stdx::thread::_ttable::access(std::this_thread::get_id());\
	if (!thread._stack.empty())\
	{\
		std::longjmp(thread._stack.top(), status);\
	}\
}\
(__VA_ARGS__)

namespace stdx::thread
{
	class _ttable
	{
		struct _tjump //_thread_state
		{
			std::stack<std::jmp_buf> _stack;
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
