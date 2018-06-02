#ifndef ATOMIC_PTR_HPP
#define ATOMIC_PTR_HPP

#include <atomic>
#include <memory> // Not needed

#include "meta.hpp"

/* shared_ptr<T>
All member functions (including copy constructor and copy assignment) can be called by multiple threads on different instances of shared_ptr without additional synchronization
even if these instances are copies and share ownership of the same object. If multiple threads of execution access the same shared_ptr without synchronization and any of those 
accesses uses a non-const member function of shared_ptr then a data race will occur; the shared_ptr overloads of atomic functions can be used to prevent the data race.
*/

namespace stdx // This may be moved to stp
{
	// http://en.cppreference.com/w/cpp/memory/shared_ptr

	using atomic_uint_largest_lock_free_t = typename stdx::meta::constrained_pack<stdx::meta::is_lock_free, stdx::meta::pack<
		std::atomic_uint_least64_t,
		std::atomic_uint_least32_t,
		std::atomic_uint_least16_t,
		std::atomic_uint_least8_t
	>>::first;

	class _atomic_ptr_ref_counter
	{
	public:
		using counter_type = atomic_uint_largest_lock_free_t;

		void increment()
		{
			_references.fetch_add(1, std::memory_order_relaxed); // No synchronization on this operation?
		}
		void decrement()
		{
			if (_references.fetch_sub(1, std::memory_order_release) == 1) // memory ordering on assignment op/reset?
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				// See https://www.boost.org/doc/libs/1_57_0/doc/html/atomic/usage_examples.html#boost_atomic.usage_examples.example_reference_counters.discussion
				// See https://stackoverflow.com/questions/14167767/what-is-the-difference-between-using-explicit-fences-and-stdatomic
				// See http://preshing.com/20131125/acquire-and-release-fences-dont-work-the-way-youd-expect/


				_delete_atomic_ptr_counter();
			}
		}
	protected:
		_atomic_ptr_ref_counter() = default;
		virtual ~_atomic_ptr_ref_counter() = default;

		virtual void _delete_atomic_ptr_counter() = 0;
	private:
		_atomic_ptr_ref_counter(_atomic_ptr_ref_counter const &) = delete;
		_atomic_ptr_ref_counter & operator=(_atomic_ptr_ref_counter const &) = delete;
		_atomic_ptr_ref_counter(_atomic_ptr_ref_counter &&) = delete;
		_atomic_ptr_ref_counter & operator=(_atomic_ptr_ref_counter &&) = delete;

		counter_type _references{ 1 };
	};

	template <class ObjType>
	class _atomic_ptr_obj_ref_counter : public _atomic_ptr_ref_counter
	{
		_atomic_ptr_obj_ref_counter<ObjType>(ObjType * object) : _atomic_ptr_ref_counter(),
			_object(object)
		{
		}

		void _delete_atomic_ptr_counter() override
		{
			if (auto object = _object.exchange(nullptr, std::memory_order_acquire))
			{
				delete object;
			}
			delete this;
		}

		std::atomic<ObjType *> _object;

		friend _atomic_ptr_ref_counter * _make_atomic_ptr_obj_counter(ObjType *);
	};

	template <class ObjType, class DelType>
	class _atomic_ptr_obj_del_ref_counter : public _atomic_ptr_obj_ref_counter<ObjType>
	{
		_atomic_ptr_obj_del_ref_counter<ObjType, DelType>(ObjType * object, DelType deleter) : _atomic_ptr_obj_ref_counter<ObjType>(object),
			_deleter(deleter)
		{
		}

		void _delete_atomic_ptr_counter() override
		{
			if (auto object = _object.exchange(nullptr, std::memory_order_relaxed)) // A load would probably suffice
			{
				_deleter(object);
			}
			delete this;
		}

		DelType _deleter;

		friend _atomic_ptr_ref_counter * _make_atomic_ptr_obj_del_counter(ObjType *, DelType);
	};

	template <class ObjType>
	inline _atomic_ptr_ref_counter * _make_atomic_ptr_obj_counter(ObjType * object)
	{
		return new _atomic_ptr_obj_ref_counter<ObjType>(object);
	}
	template <class ObjType, class DelType>
	inline _atomic_ptr_ref_counter * _make_atomic_ptr_obj_del_counter(ObjType * object, DelType deleter)
	{
		return new _atomic_ptr_obj_del_ref_counter<ObjType, DelType>(object, std::move(deleter));
	}

	template <class Type>
	class atomic_ptr
	{
	public:
		atomic_ptr<Type>() = default;
		template <
			class ObjType, 
			std::enable_if_t<
				std::conditional_t<std::is_array_v<Type>,
					std::is_convertible<Type *, std::remove_extent<ObjType> *>,
					std::is_convertible<Type *, ObjType *>>::value,
				int> = 0>
		atomic_ptr<Type>(ObjType * object) :
			_counter(_make_atomic_ptr_obj_counter(object))
		{
		}
		template <
			class ObjType,
			class DelType,
			std::enable_if_t<
				std::conjunction_v<
					std::conditional_t<std::is_array_v<Type>,
						std::is_convertible<Type *, std::remove_extent<ObjType> *>,
						std::is_convertible<Type *, ObjType *>>,
					std::conjunction<
						std::is_move_constructible<DelType>, 
						std::is_invocable<DelType, ObjType *>>>,
				int> = 0>
		atomic_ptr<Type>(ObjType * object, DelType deleter) :
			_counter(_make_atomic_ptr_obj_del_counter(object, std::move(deleter)))
		{
		}
		atomic_ptr<Type>(atomic_ptr<Type> const & other)
		{
			other._access.fetch_add(1, std::memory_order_relaxed);

			if (auto control = other._control.load(std::memory_order_acquire))
			{
				control->increment();

				_control.store(control, std::memory_order_release);
			}

			other._access.fetch_sub(1, std::memory_order_relaxed); // Is there any guarantee that these operations are not reordered?
		}
		atomic_ptr<Type> & operator=(atomic_ptr<Type> const & other)
		{
			
		}
		~atomic_ptr<Type>()
		{
			reset();
		}

		void reset()
		{
			if (auto control = _control.exchange(nullptr, std::memory_order_acq_rel))
			{
				while (_access.load(std::memory_order_relaxed));

				control->decrement();
			}
		}
		template <class ObjType>
		void reset(ObjType * object)
		{
			reset();

			_control.store(_make_atomic_ptr_obj_counter(object), std::memory_order_relaxed);
		}
		template <class ObjType, class DelType>
		void reset(ObjType * object, DelType deleter)
		{
			reset();

			_control.store(_make_atomic_ptr_obj_del_counter(object, std::move(deleter)), std::memory_order_relaxed);
		}
		
		operator bool()
		{
			return _counter.load(std::memory_order_relaxed);
		}
	private:
		_atomic_ptr_ref_counter::counter_type _access{ 0 };
		std::atomic<_atomic_ptr_ref_counter *> _control{ nullptr };
	};
}

#endif
