#ifndef STDX_header_THREAD
#define STDX_header_THREAD

#include <thread>

namespace stdx::this_thread
{
}

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
