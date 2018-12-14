#ifndef STDX_header_CONCURRENT_QUEUE
#define STDX_header_CONCURRENT_QUEUE

#include <atomic>

#include "atomic_ptr.hpp"

namespace stdx
{
	using _uint_largest_lock_free_t =
		typename stdx::meta::constrained_pack<
			stdx::meta::is_lock_free,
			stdx::meta::pack<
				std::atomic_uint_least64_t,
				std::atomic_uint_least32_t,
				std::atomic_uint_least16_t,
				std::atomic_uint_least8_t
			>
		>::template push<
			std::atomic_uint_least64_t
		>::first::value_type;

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
			auto size = _size.fetch_add(1, std::memory_order_acquire); // fetch_add(1)/fetch_add(0) vs load/store (acq/rel)

			stdx::atomic_ptr<_node> node(new _node(element)); // Need to allocate from memory pool, otherwise lockfree-ness is lost (explore synchronized_pool_resource)
			stdx::atomic_ptr<_node> back_actual(nullptr); // To be used with compare_swap

			bool non_empty = false;
			while (!_back.compare_swap(back_actual, node, std::memory_order_relaxed))
			{
				non_empty = true;
			}
			if (non_empty)
			{
				node_actual->next = node; // We need to guarantee that no one else touches this ptr
			}

			// TODO update next
		}
		bool pop(Type &)
		{
			stdx::atomic_ptr<_node> element = _front;

			while (element)
			{
				if (!element->popped.test_and_set(std::memory_order_relaxed))
				{
					// TODO

				}
				else
				{


				}
			}
		}
	private:
		struct _node
		{
			_node(Type const & element) : 
				object(element)
			{
			}

			stdx::atomic_ptr<_node> next;
			std::atomic_flag popped = ATOMIC_FLAG_INIT;

			Type object;
		};
	
		stdx::atomic_ptr<_node> _front;
		stdx::atomic_ptr<_node> _back;
		std::atomic<_uint_largest_lock_free_t> _size;

		std::atomic<_uint_largest_lock_free_t> _pop_counter;
	};
}

#endif
