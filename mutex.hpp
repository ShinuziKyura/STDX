#ifndef STDX_header_MUTEX
#define STDX_header_MUTEX

#include <limits>
#include <thread>
#include <mutex>
#include <atomic>

#include "meta.hpp"

namespace stdx
{
#if !(defined(STDX_NAMESPACE_THREAD) || defined(STDX_NAMESPACE_ALL))
inline
#endif
namespace thread
{	
	class ranked_mutex // Underlying type: std::mutex
	{
	public:
		ranked_mutex(std::size_t rank) noexcept :
			_rank(rank)
		{
		}
		ranked_mutex(ranked_mutex const &) = delete;
		ranked_mutex & operator=(ranked_mutex const &) = delete;
		ranked_mutex(ranked_mutex &&) = delete;
		ranked_mutex & operator=(ranked_mutex &&) = delete;

		void lock()
		{
			_check_next_mutex();

			_mutex.lock();

			_previous_mutex = _current_mutex;
			_current_mutex = this;
		}
		bool try_lock()
		{
			_check_next_mutex();

			if (_mutex.try_lock())
			{
				_previous_mutex = _current_mutex;
				_current_mutex = this;

				return true;
			}

			return false;
		}
		void unlock()
		{
			_check_current_mutex();

			_current_mutex = _previous_mutex;

			_mutex.unlock();
		}
		std::size_t rank() const
		{
			return _rank;
		}
	private:
		void _check_next_mutex()
		{
			if (_current_mutex && _current_mutex->_rank >= _rank) // Only allow totally ordered mutexes
			{
				_unlock_previous_mutexes();
				throw std::logic_error("Thread rank invariant violated on lock / try_lock!");
			}
		}
		void _check_current_mutex()
		{
			if (_current_mutex != this)
			{
				_unlock_previous_mutexes();
				throw std::logic_error("Thread rank invariant violated on unlock!");
			}
		}
		void _unlock_previous_mutexes()
		{
			while (auto mutex = _current_mutex)
			{
				_current_mutex = _previous_mutex;
				mutex->unlock();
			}
		}

		thread_local static inline ranked_mutex * _current_mutex = nullptr;
		ranked_mutex * _previous_mutex = nullptr;
		std::size_t const _rank;
		std::mutex _mutex;
	};

	class spin_mutex
	{
	public:
		spin_mutex() noexcept = default;
		spin_mutex(spin_mutex const &) = delete;
		spin_mutex & operator=(spin_mutex const &) = delete;
		spin_mutex(spin_mutex &&) = delete;
		spin_mutex & operator=(spin_mutex &&) = delete;
	
		void lock()
		{
			while (_mutex.test_and_set(std::memory_order_acquire))
			{
				std::this_thread::yield();
			}
		}
		bool try_lock()
		{
			return !_mutex.test_and_set(std::memory_order_acquire);
		}
		void unlock()
		{
			_mutex.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag _mutex = ATOMIC_FLAG_INIT;
	};

	class shared_spin_mutex
	{
		using _atomic_int_largest_lock_free_t =
			stdx::meta::type_if<stdx::meta::is_lock_free_v<std::atomic_int_least64_t>>::then<
				std::atomic_int_least64_t
			>::else_if<stdx::meta::is_lock_free_v<std::atomic_int_least32_t>>::then<
				std::atomic_int_least32_t
			>::else_if<stdx::meta::is_lock_free_v<std::atomic_int_least16_t>>::then<
				std::atomic_int_least16_t
			>::else_if<stdx::meta::is_lock_free_v<std::atomic_int_least8_t>>::then<
				std::atomic_int_least8_t
			>::else_then<
				std::atomic_int_least64_t
			>::end_if;
		using _int_largest_lock_free_t =
			_atomic_int_largest_lock_free_t::value_type;
	public:
		shared_spin_mutex() noexcept = default;
		shared_spin_mutex(shared_spin_mutex const &) = delete;
		shared_spin_mutex & operator=(shared_spin_mutex const &) = delete;
		shared_spin_mutex(shared_spin_mutex &&) = delete;
		shared_spin_mutex & operator=(shared_spin_mutex &&) = delete;

		void lock()
		{
			_int_largest_lock_free_t mutex_state = 0;

			while (!_mutex.compare_exchange_weak(mutex_state, _exclusive_state, std::memory_order_acquire))
			{
				mutex_state = 0;

				std::this_thread::yield();
			}
		}
		bool try_lock()
		{
			_int_largest_lock_free_t mutex_state = 0;

			return _mutex.compare_exchange_weak(mutex_state, _exclusive_state, std::memory_order_acquire);
		}
		void unlock()
		{
			_mutex.store(0, std::memory_order_release);
		}
		void lock_shared()
		{
			_int_largest_lock_free_t mutex_state = _mutex.load(std::memory_order_relaxed);

			do
			{
				if (ensure_shared_state(mutex_state))
				{
					std::this_thread::yield();
				}
			}
			while (!_mutex.compare_exchange_weak(mutex_state, mutex_state + 1, std::memory_order_acquire));
		}

		bool try_lock_shared()
		{
			_int_largest_lock_free_t mutex_state = _mutex.load(std::memory_order_relaxed);
			
			ensure_shared_state(mutex_state);

			return _mutex.compare_exchange_weak(mutex_state, mutex_state + 1, std::memory_order_acquire);
		}
		void unlock_shared()
		{
			_mutex.fetch_sub(1, std::memory_order_release);
		}
		bool upgrade()
		{
			std::unique_lock<spin_mutex> const upgrade_lock(_upgrade_mutex, std::try_to_lock);
			
			if (upgrade_lock.owns_lock())
			{
				_int_largest_lock_free_t mutex_state = 1;

				if (!_mutex.compare_exchange_strong(mutex_state, _exclusive_state, std::memory_order_acquire)) // Edge case, will avoid loops when only this thread holds the mutex
				{
					while (!_mutex.compare_exchange_weak(mutex_state, _exclusive_state + mutex_state - 1, std::memory_order_acquire));

					while (_mutex.load(std::memory_order_acquire) != _exclusive_state)
					{
						std::this_thread::yield();
					}
				}
			}

			return upgrade_lock.owns_lock();
		}
		void downgrade()
		{
			_mutex.store(1, std::memory_order_release);
		}

		static constexpr bool is_lock_free()
		{
			return stdx::meta::is_lock_free<_atomic_int_largest_lock_free_t>::value;
		}
	private:
		static constexpr auto _exclusive_state = std::numeric_limits<_int_largest_lock_free_t>::min();

		// Ensures state is valid for shared locking, returns whether mutex_state was modified
		static bool ensure_shared_state(_int_largest_lock_free_t & mutex_state)
		{
			if (mutex_state < 0)
			{
				mutex_state = 0;
				return true;
			}
			if (mutex_state == std::numeric_limits<_int_largest_lock_free_t>::max()) // Edge case, we'll try decrementing now; we might get lucky on this attempt if one thread releases the mutex
			{
				--mutex_state;
				return true;
			}
			return false;
		}

		_atomic_int_largest_lock_free_t _mutex = 0;
		spin_mutex _upgrade_mutex;
	};
}
}

#endif
