#ifndef STDX_THREAD_HPP
#define STDX_THREAD_HPP

#include <thread>

namespace stdx::thread
{
	template <class FuncType, class ... ArgTypes>
	constexpr void daemon(FuncType func, ArgTypes && ... args)
	{
		std::thread(func, std::forward<ArgTypes>(args) ...).detach();
	}
}

#if defined(STDX_USING_THREAD) || defined(STDX_USING_ALL)
namespace stdx { using namespace thread; }
#endif

#endif