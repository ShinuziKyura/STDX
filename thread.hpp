#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <csetjmp>

#include <stack>
#include <thread>

namespace stdx::this_thread
{
	inline auto & get_jmp_state()
	{
		thread_local class _jmp_state
		{
			struct _jmp_buf
			{
				std::jmp_buf _buf;
			};
		public:
			std::jmp_buf & emplace()
			{
				return _jmp_env.emplace()._buf;
			}
			std::jmp_buf & top()
			{
				return _jmp_env.top()._buf;
			}
			void pop()
			{
				_jmp_env.pop();
			}
			bool empty()
			{
				return _jmp_env.empty();
			}
			int & status()
			{
				return _jmp_status;
			}
		private:
			std::stack<_jmp_buf> _jmp_env;
			int _jmp_status;
		} 
		jmp_state;
		return jmp_state;
	}
};

namespace stdx::thread
{
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
