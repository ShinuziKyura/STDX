#ifndef CONCURRENT_QUEUE_HPP
#define CONCURRENT_QUEUE_HPP

#include <atomic>
#include "atomic_ptr.hpp"

namespace stdx
{
	template <class Type, class CompareLess = std::less<Type>, class CompareEqual = std::equal_to<Type>>
	class concurrent_queue
	{
	//	using _atomic_counter = _atomic_lock_free_uint_fast_largest_t;
	public:
		/*concurrent_queue<Type, CompareLess, CompareEqual>()
		{
			
		}//*/
		/*concurrent_queue<Type, CompareLess, CompareEqual>(CompareLess const & compare_less, CompareEqual const & compare_equal)
		{
		}
		concurrent_queue<Type, CompareLess, CompareEqual>(concurrent_queue const & other)
		{
		}
		concurrent_queue<Type, CompareLess, CompareEqual> & operator=(concurrent_queue const & other)
		{
		}
		concurrent_queue<Type, CompareLess, CompareEqual>(concurrent_queue && other)
		{
		}
		concurrent_queue<Type, CompareLess, CompareEqual> & operator=(concurrent_queue && other)
		{
		}
		~concurrent_queue<Type, CompareLess, CompareEqual>()
		{
		}*/

		Type pop()
		{
			auto prev = _concurrent_queue_top;
		}
	private:
		struct _concurrent_node
		{
			atomic_ptr<_concurrent_node> next{ nullptr };
		};
		struct _concurrent_obj_node : _concurrent_node
		{
			Type object;
		};

	/*	_atomic_counter _concurrent_queue_push_count{ 0 };
		_atomic_counter _concurrent_queue_pop_count{ 0 };
		_atomic_counter _concurrent_queue_object_count{ 0 }; */
	
		atomic_ptr<_concurrent_node> _concurrent_queue_top{ new _concurrent_node() };
	};
}

#endif//CONCURRENT_QUEUE_HPP
