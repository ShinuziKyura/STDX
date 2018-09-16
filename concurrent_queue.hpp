#ifndef STDX_CONCURRENT_QUEUE_HPP
#define STDX_CONCURRENT_QUEUE_HPP

#include <atomic>

#include "atomic_ptr.hpp"

namespace stdx
{
	template <class Type, class Ordering>
	class concurrent_queue;

	template <class Type>
	class concurrent_queue<Type, std::equal_to<Type>>
	{
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

		void push(Type const & element)
		{
			stdx::atomic_ptr<_node> node(new _node(element));
			stdx::atomic_ptr<_node> node_actual; // To be used with compare_swap
			/*
				swap _back, expect node_actual, desired is node, if expected is different, we try with node_actual->next
			*/
		}
		Type pop()
		{
			stdx::atomic_ptr<_node> element = _front;

			while (element)
			{
				if (!element->flag.test_and_set(std::memory_order_relaxed))
				{


				}
				else
				{


				}
			}
		}
	private:
		enum class _node_state
		{
			popped,
			pushed,
			null
		};
		struct _node
		{
			_node(Type const & element) : 
				object(element)
			{
			}

			stdx::atomic_ptr<_node> next;
			std::atomic_flag flag = ATOMIC_FLAG_INIT;

			Type object;
		};
	
		stdx::atomic_ptr<_node> _front;
		stdx::atomic_ptr<_node> _back;

		std::atomic<stdx::_uint_largest_lock_free_t> _pop_counter;
	};
}

#endif
