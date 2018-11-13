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
	return !::stdx::thread::_ttable::access(std::this_thread::get_id())._status;\
}\
()

// Checks if next return will jump
#define STDX_THREAD_JUMP_CHECK_RETURN() \
[]\
{\
	return !::stdx::thread::_ttable::access(std::this_thread::get_id())._context.empty();\
}\
()

// Executes invocation passed as argument while setting a point to return to without actually returning from that function
#define STDX_THREAD_JUMP_INVOKE(...) \
[&] () -> decltype(__VA_ARGS__)\
{\
	auto & STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__) = ::stdx::thread::_ttable::access(std::this_thread::get_id());\
	struct STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_TYPE_, __LINE__)\
	{\
		decltype(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)) STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_THREAD_VARIABLE_, __LINE__);\
		STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_TYPE_, __LINE__)(decltype(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)) STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_THREAD_VARIABLE_INITIALIZER_, __LINE__)) :\
			STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_THREAD_VARIABLE_, __LINE__)(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_THREAD_VARIABLE_INITIALIZER_, __LINE__))\
		{\
		}\
		~STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_TYPE_, __LINE__)()\
		{\
			STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_THREAD_VARIABLE_, __LINE__)._context.pop();\
		}\
	};\
	STDX_SCOPED_VARIABLE(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STRUCT_TYPE_, __LINE__)(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)));\
	switch (int volatile STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STATUS_VARIABLE_, __LINE__) = setjmp(STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)._context.emplace()))\
	{\
		case 0:\
		{\
			STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)._status = 0;\
			return __VA_ARGS__;\
		}\
		default:\
		{\
			STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_THREAD_VARIABLE_, __LINE__)._status = STDX_CONCATENATE(STDX_THREAD_JUMP_INVOKE_STATUS_VARIABLE_, __LINE__);\
		}\
	}\
}\
()

// Returns execution to last point in stack without returning from current function
#define STDX_THREAD_JUMP_RETURN(...) \
[] (int volatile STDX_CONCATENATE(STDX_THREAD_JUMP_RETURN_STATUS_VARIABLE_, __LINE__) = 0)\
{\
	auto & STDX_CONCATENATE(STDX_THREAD_JUMP_RETURN_THREAD_VARIABLE_, __LINE__) = ::stdx::thread::_ttable::access(std::this_thread::get_id());\
	if (!STDX_CONCATENATE(STDX_THREAD_JUMP_RETURN_THREAD_VARIABLE_, __LINE__)._context.empty())\
	{\
		std::longjmp(STDX_CONCATENATE(STDX_THREAD_JUMP_RETURN_THREAD_VARIABLE_, __LINE__)._context.top(), STDX_CONCATENATE(STDX_THREAD_JUMP_RETURN_STATUS_VARIABLE_, __LINE__));\
	}\
}\
(__VA_ARGS__)

namespace stdx::thread
{
	class _ttable
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
