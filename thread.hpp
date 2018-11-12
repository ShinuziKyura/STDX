#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>
#include <type_traits>
#include <thread>
#include <mutex>
#include <unordered_map>

#include "utility.hpp"

#ifdef STDX_THREAD_SUPPRESS_WARNINGS
#if defined(_MSC_VER)
#pragma warning(disable: 4611)
#pragma warning(disable: 4715)
#else
#endif
#endif

#define STDX_THREAD_JUMP_INVOKE(...) [&] () -> decltype(__VA_ARGS__) { auto & _thread = ::stdx::thread::_ttable::access(std::this_thread::get_id()); _thread._jumped = false; if (!setjmp(_thread._jump)) return __VA_ARGS__; else _thread._jumped = true; }()
#define STDX_THREAD_JUMP_RETURN() std::longjmp(::stdx::thread::_ttable::access(std::this_thread::get_id())._jump, 0)

namespace stdx::thread
{
	class _ttable
	{
		struct _tjump
		{
			std::jmp_buf _jump;
			bool _jumped = false;
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
