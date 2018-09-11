#ifndef STDX_CONCURRENT_QUEUE_HPP
#define STDX_CONCURRENT_QUEUE_HPP

#include <atomic>
#include "synchronized_ptr.hpp"

namespace stdx
{
	template <class Type, class Ordering>
	class concurrent_queue;

	template <class Type>
	class concurrent_queue<Type, std::equal_to<Type>>
	{
	//	using _atomic_counter = _atomic_lock_free_uint_fast_largest_t;
	public:
		concurrent_queue()
		{
			
		}
		concurrent_queue(concurrent_queue const & other)
		{
		}
		concurrent_queue & operator=(concurrent_queue const & other)
		{
		}
		concurrent_queue(concurrent_queue && other)
		{
		}
		concurrent_queue & operator=(concurrent_queue && other)
		{
		}
		~concurrent_queue()
		{
		}

		Type pop()
		{

		}
	private:
		struct _node
		{
			stdx::synchronized_ptr<_node> next;
			std::atomic_flag flag = ATOMIC_FLAG_INIT;
			Type object;
		};
	
		stdx::synchronized_ptr<_node> _top;
	};
}

#endif
