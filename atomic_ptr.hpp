#ifndef STDX_ATOMIC_PTR_HPP
#define STDX_ATOMIC_PTR_HPP

#include <atomic>
#include <typeinfo>
#include <memory> // Not needed

#include "meta.hpp"

/* shared_ptr<T>
All member functions (including copy constructor and copy assignment) can be called by multiple threads on different instances of shared_ptr without additional synchronization
even if these instances are copies and share ownership of the same object. If multiple threads of execution access the same shared_ptr without synchronization and any of those 
accesses uses a non-const member function of shared_ptr then a data race will occur; the shared_ptr overloads of synchronized functions can be used to prevent the data race.
*/

// http://en.cppreference.com/w/cpp/memory/shared_ptr
// Note to self: check https://blogs.msdn.microsoft.com/vcblog/2018/04/09/intellisense-for-remote-linux-headers/?utm_source=vs_developer_news&utm_medium=referral

namespace stdx // This may be moved to stp
{
	/* Unsigned integer type T at least as large as the largest size for which std::atomic<T> is lock free.
	 * If there is no type T for which std::atomic<T> is lock free, T is the largest size supported by the architecture.
	 */
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

	// Reference counter abstract class

	class _atomic_ptr_ref_counter
	{
	protected:
		_atomic_ptr_ref_counter() noexcept = default;
		_atomic_ptr_ref_counter(_atomic_ptr_ref_counter const &) = delete;
		_atomic_ptr_ref_counter & operator=(_atomic_ptr_ref_counter const &) = delete;
		_atomic_ptr_ref_counter(_atomic_ptr_ref_counter &&) = delete;
		_atomic_ptr_ref_counter & operator=(_atomic_ptr_ref_counter &&) = delete;
		virtual ~_atomic_ptr_ref_counter() noexcept = default;
	public:
		void increment() noexcept
		{
			_references.fetch_add(1, std::memory_order_relaxed);
		}
		void decrement() noexcept
		{
			if (_references.fetch_sub(1, std::memory_order_release) == 1) // ^^^^
			{
				std::atomic_thread_fence(std::memory_order_acquire); // vvvv
				// https://www.boost.org/doc/libs/1_57_0/doc/html/synchronized/usage_examples.html#boost_synchronized.usage_examples.example_reference_counters.discussion
				// https://stackoverflow.com/questions/14167767/what-is-the-difference-inside-using-explicit-fences-and-stdsynchronized
				// http://preshing.com/20120625/memory-ordering-at-compile-time/
				// http://preshing.com/20120710/memory-barriers-are-like-source-control-operations/
				// http://preshing.com/20120913/acquire-and-release-semantics/
				// http://preshing.com/20130930/double-checked-locking-is-fixed-in-cpp11/

				_delete();
			}
		}
		auto use_count() const noexcept -> _uint_largest_lock_free_t
		{
			return _references.load(std::memory_order_relaxed);
		}

		virtual void * get() const noexcept = 0;
		virtual void * get_deleter(type_info const &) noexcept = 0;
	protected:
		virtual void _delete() noexcept = 0;

		std::atomic<_uint_largest_lock_free_t> _references{ 1 };
	};

	// Reference counter object container subclass

	template <class ObjType>
	class _atomic_ptr_obj_ref_counter : public _atomic_ptr_ref_counter
	{
	protected:
		_atomic_ptr_obj_ref_counter(ObjType * object) noexcept : _atomic_ptr_ref_counter(),
			_object(object)
		{
		}
	public:
		void * get() const noexcept
		{
			return _object;
		}
		void * get_deleter(type_info const &) noexcept
		{
			return nullptr;
		}
	protected:
		void _delete() noexcept
		{
			delete _object;

			delete this;
		}

		ObjType * const _object;

		template <class Type>
		friend class atomic_ptr;
	};

	// Reference counter object and deleter container subclass

	template <class ObjType, class DelType>
	class _atomic_ptr_obj_del_ref_counter : public _atomic_ptr_obj_ref_counter<ObjType>
	{
	protected:
		_atomic_ptr_obj_del_ref_counter(ObjType * object, DelType deleter) noexcept : _atomic_ptr_obj_ref_counter<ObjType>(object),
			_deleter(std::move(deleter))
		{
		}
	public:
		void * get_deleter(type_info const & del_type) noexcept
		{
			if (del_type == typeid(DelType))
			{
				return static_cast<void *>(std::addressof(_deleter));
			}
			return nullptr;
		}
	protected:
		void _delete() noexcept
		{
			_deleter(this->_object);

			delete this;
		}

		DelType _deleter;

		template <class Type>
		friend class atomic_ptr;
	};

	// Reference counter object, deleter and allocator container subclass

	// TODO
	template <class ObjType, class DelType, class AllocType>
	class _atomic_ptr_obj_del_alloc_ref_counter; // : public _synchronized_ptr_obj_del_ref_counter<ObjType, DelType>

	// ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ==== ====

	template <class Type>
	class atomic_ptr;

	// Access counter class

	template <class Type>
	class _atomic_ptr_acc_counter
	{
		using element_type = std::remove_extent_t<Type>;

		_atomic_ptr_acc_counter(atomic_ptr<Type> const * atomic_ptr) noexcept :
			_atomic_ptr(atomic_ptr)
		{
			_atomic_ptr->_accesses.fetch_add(1, std::memory_order_acquire); // vvvv
		}
	public:
		~_atomic_ptr_acc_counter() noexcept
		{
			_atomic_ptr->_accesses.fetch_sub(1, std::memory_order_release); // ^^^^
		}

		auto operator*() const noexcept -> Type &
		{
			return *static_cast<Type *>(_atomic_ptr->_ref_counter.load(std::memory_order_relaxed)->get());
		}
		auto operator->() const noexcept -> Type *
		{
			return static_cast<Type *>(_atomic_ptr->_ref_counter.load(std::memory_order_relaxed)->get());
		}
		auto operator[](std::ptrdiff_t index) const -> element_type &
		{
			return static_cast<element_type *>(_atomic_ptr->_ref_counter.load(std::memory_order_relaxed)->get())[index];
		}
		explicit operator bool() const noexcept
		{
			return _atomic_ptr->_ref_counter.load(std::memory_order_relaxed) != nullptr;
		}
	private:
		atomic_ptr<Type> const * _atomic_ptr;

		friend class atomic_ptr<Type>;
	};

	// synchronized pointer class

	// TODO test and prove correctness
	template <class Type>
	class atomic_ptr
	{
	public:
		using element_type = std::remove_extent_t<Type>;
		
		constexpr atomic_ptr() noexcept = default;
		template <class ObjType, 
			std::enable_if_t<
				std::conditional_t<std::is_array_v<Type>,
					std::is_convertible<Type *, std::remove_extent<ObjType> *>,
					std::is_convertible<Type *, ObjType *>
				>::value,
				int
			> = 0>
		atomic_ptr(ObjType * object) :
			_ref_counter(_make_atomic_ptr_ref_counter(object))
		{
		}
		template <class ObjType, class DelType,
			std::enable_if_t<
				std::conjunction_v<
					std::conditional_t<std::is_array_v<Type>,
						std::is_convertible<Type *, std::remove_extent<ObjType> *>,
						std::is_convertible<Type *, ObjType *>
					>,
					std::conjunction<
						std::is_move_constructible<DelType>, 
						std::is_invocable<DelType, ObjType *>
					>
				>,
				int
			> = 0>
		atomic_ptr(ObjType * object, DelType deleter) :
			_ref_counter(_make_atomic_ptr_ref_counter(object, std::move(deleter)))
		{
		}
		atomic_ptr(atomic_ptr const & other) noexcept :
			_ref_counter(other._acquire())
		{
		}
		atomic_ptr & operator=(atomic_ptr const & other) noexcept
		{
			_release(other._acquire());
			return *this;
		}
		atomic_ptr(atomic_ptr && other)
		{
			// TODO
		}
		atomic_ptr & operator=(atomic_ptr && other)
		{
			// TODO
		}
		~atomic_ptr() noexcept
		{
			_release(nullptr);
		}

		// Modifiers
		void reset() noexcept
		{
			_release(nullptr);
		}
		template <class ObjType>
		auto reset(ObjType * object) -> /* void */
			std::enable_if_t<
				std::conditional_t<std::is_array_v<Type>, 
					std::is_convertible<Type *, std::remove_extent<ObjType> *>, 
					std::is_convertible<Type *, ObjType *>
				>::value
			>
		{
			_release(_make_atomic_ptr_ref_counter(object));
		}
		template <class ObjType, class DelType>
		auto reset(ObjType * object, DelType deleter) -> /* void */
			std::enable_if_t<
				std::conjunction_v<
					std::conditional_t<std::is_array_v<Type>,
						std::is_convertible<Type *, std::remove_extent<ObjType> *>,
						std::is_convertible<Type *, ObjType *>
					>,
					std::conjunction<
						std::is_move_constructible<DelType>, 
						std::is_invocable<DelType, ObjType *>
					>
				>
			>
		{
			_release(_make_atomic_ptr_ref_counter(object, std::move(deleter)));
		}
		// Consider renaming these exchange
		void swap(atomic_ptr & other)
		{
			// TODO
		}
		auto compare_swap(atomic_ptr & expected, atomic_ptr desired, std::memory_order success, std::memory_order failure) noexcept -> bool
		{
			// TODO
		}
		auto compare_swap(atomic_ptr & expected, atomic_ptr desired, std::memory_order order = std::memory_order_seq_cst) noexcept -> bool
		{
			// TODO
		}

		// Observers - single-thread
		auto operator*() const noexcept -> Type &
		{
			return _atomic_ptr_acc_counter<Type>(this).operator*();
		}
		auto operator->() const noexcept -> Type *
		{
			return _atomic_ptr_acc_counter<Type>(this).operator->();
		}
		auto operator[](std::ptrdiff_t index) const -> element_type &
		{
			return _atomic_ptr_acc_counter<Type>(this).operator[](index);
		}
		explicit operator bool() const noexcept
		{
			return _atomic_ptr_acc_counter<Type>(this).operator bool();
		}

		// Observers - multi-thread
		auto get() const noexcept
		{
			return _atomic_ptr_acc_counter<Type>(this);
		}
		auto use_count() const noexcept -> _uint_largest_lock_free_t
		{
			_atomic_ptr_acc_counter<Type> acc_counter(this);
			
			if (auto const ref_counter = _ref_counter.load(std::memory_order_relaxed))
			{
				return ref_counter->use_count();
			}

			return 0;
		}
		auto owner_before(atomic_ptr const & other) const noexcept -> bool
		{
			_atomic_ptr_acc_counter<Type> this_acc_counter(this);
			_atomic_ptr_acc_counter<Type> other_acc_counter(&other);

			return _ref_counter.load(std::memory_order_relaxed) < other._ref_counter.load(std::memory_order_relaxed);
		}

		static constexpr auto is_lock_free() noexcept -> bool
		{
			return stdx::meta::is_lock_free<std::atomic<_atomic_ptr_ref_counter *>>::value && stdx::meta::is_lock_free<std::atomic<_uint_largest_lock_free_t>>::value;
		}
	private:
		auto _get_deleter(type_info const & del_type) const noexcept -> void *
		{
			_atomic_ptr_acc_counter<Type> acc_counter(this);

			return _ref_counter.load(std::memory_order_relaxed)->get_deleter(del_type);
		}
		auto _acquire() noexcept -> _atomic_ptr_ref_counter *
		{
			_accesses.fetch_add(1, std::memory_order_acquire); // vvvv

			auto const ref_counter = _ref_counter.load(std::memory_order_relaxed);

			if (ref_counter)
			{
				ref_counter->increment();
			}

			_accesses.fetch_sub(1, std::memory_order_release); // ^^^^

			return ref_counter;
		}
		void _release(_atomic_ptr_ref_counter * new_ref_counter) noexcept
		{
			if (auto const old_ref_counter = _ref_counter.exchange(new_ref_counter, std::memory_order_relaxed))
			{
				_uint_largest_lock_free_t accesses = 0;

				while (!_accesses.compare_exchange_weak(accesses, 0, std::memory_order_acq_rel)) // vvvv ^^^^
				{
					accesses = 0;
				}

				old_ref_counter->decrement();
			}
		}

		template <class ObjType>
		static auto _make_atomic_ptr_ref_counter(ObjType * object) -> _atomic_ptr_ref_counter *
		{
			return new _atomic_ptr_obj_ref_counter<ObjType>(object);
		}
		template <class ObjType, class DelType>
		static auto _make_atomic_ptr_ref_counter(ObjType * object, DelType deleter) -> _atomic_ptr_ref_counter *
		{
			return new _atomic_ptr_obj_del_ref_counter<ObjType, DelType>(object, std::move(deleter));
		}

		std::atomic<_atomic_ptr_ref_counter *> _ref_counter{ nullptr }; // Payload
		std::atomic<_uint_largest_lock_free_t> mutable _accesses{ 0 }; // Guard

		template <class DelType1, class Type1>
		friend DelType1 * get_deleter(atomic_ptr<Type1> const &) noexcept;

		friend class _atomic_ptr_acc_counter<Type>;
	};

	// TODO
	template<class Type, class ... ArgTypes>
	auto make_synchronized(ArgTypes && ... args) ->
		std::enable_if_t<
			!std::is_array_v<Type>,
			atomic_ptr<Type>
		>;

	template<class Type>
	atomic_ptr<Type> make_synchronized(std::size_t size);

	template<class Type>
	atomic_ptr<Type> make_synchronized();
	
	template<class Type>
	atomic_ptr<Type> make_synchronized(std::size_t size, std::remove_extent_t<Type> const & val);
	
	template<class Type>
	atomic_ptr<Type> make_synchronized(std::remove_extent_t<Type> const & val);

	// TODO
	// allocate_shared

/*	template<class Type, class OtherType>
	synchronized_ptr<Type> static_pointer_cast(synchronized_ptr<OtherType> const & other) noexcept
	{

	}

	template<class Type, class OtherType>
	synchronized_ptr<Type> dynamic_pointer_cast(synchronized_ptr<OtherType> const & other) noexcept
	{

	}
	
	template<class Type, class OtherType>
	synchronized_ptr<Type> const_pointer_cast(synchronized_ptr<OtherType> const & other) noexcept
	{

	}
	
	template<class Type, class OtherType>
	synchronized_ptr<Type> reinterpret_pointer_cast(synchronized_ptr<OtherType> const & other) noexcept
	{

	} */

	template <class DelType, class Type>
	DelType * get_deleter(atomic_ptr<Type> const & atomic_ptr) noexcept
	{
		return static_cast<DelType *>(atomic_ptr._get_deleter(typeid(DelType)));
	}
}

#endif
