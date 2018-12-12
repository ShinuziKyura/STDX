#ifndef STDX_implementation_MUTEX_HEADER
#define STDX_implementation_MUTEX_HEADER

#include <limits>
#include <thread>
#include <mutex>
#include <atomic>

#include "meta.hpp"
#include "utility.hpp"

namespace stdx::mutex
{	
	class ranked_mutex
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
				std::logic_error("Thread rank invariant violated on lock / try_lock!");
			}
		}
		void _check_current_mutex()
		{
			if (_current_mutex != this)
			{
				_unlock_previous_mutexes();
				std::logic_error("Thread rank invariant violated on unlock!");
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
			stdx::meta::constrained_pack<
				stdx::meta::is_lock_free,
				stdx::meta::pack<
					std::atomic_int_least64_t,
					std::atomic_int_least32_t,
					std::atomic_int_least16_t,
					std::atomic_int_least8_t
				>
			>::push<
				std::atomic_int_least64_t
			>::first;
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
				if (mutex_state < 0)
				{
					mutex_state = 0;
					std::this_thread::yield();
				}
				else if (mutex_state == _shared_state_limit) // Edge case, we'll try decrementing now; we might get lucky on this attempt if one thread releases the mutex
				{
					--mutex_state;
					std::this_thread::yield();
				}
			}
			while (!_mutex.compare_exchange_weak(mutex_state, mutex_state + 1, std::memory_order_acquire));
		}
		bool try_lock_shared()
		{
			_int_largest_lock_free_t mutex_state = _mutex.load(std::memory_order_relaxed);
			
			if (mutex_state < 0)
			{
				mutex_state = 0;
			}
			else if (mutex_state == _shared_state_limit)
			{
				--mutex_state;
			}

			return _mutex.compare_exchange_weak(mutex_state, mutex_state + 1, std::memory_order_acquire);
		}
		void unlock_shared()
		{
			_mutex.fetch_sub(1, std::memory_order_release);
		}
		void upgrade()
		{
			STDX_SCOPED_VARIABLE(std::scoped_lock<spin_mutex>(_upgrade_mutex));

			_int_largest_lock_free_t mutex_state = 1;

			if (!_mutex.compare_exchange_strong(mutex_state, _exclusive_state, std::memory_order_acquire)) // Edge case, will avoid loops when single thread holds the mutex in shared mode
			{
				while (!_mutex.compare_exchange_weak(mutex_state, _exclusive_state + mutex_state - 1, std::memory_order_acquire));

				while (_mutex.load(std::memory_order_acquire) != _exclusive_state)
				{
					std::this_thread::yield();
				}
			}
		}
		void downgrade()
		{
			_mutex.store(1, std::memory_order_release);
		}

		static constexpr bool is_lock_free()
		{
			return stdx::meta::is_lock_free<_atomic_int_largest_lock_free_t>::value;
		}
		static constexpr auto maximum_number_of_owners()
		{
			return _shared_state_limit;
		}
	private:
		static constexpr auto _shared_state_limit = std::numeric_limits<_int_largest_lock_free_t>::max();
		static constexpr auto _exclusive_state = std::numeric_limits<_int_largest_lock_free_t>::min();

		_atomic_int_largest_lock_free_t _mutex = 0;
		spin_mutex _upgrade_mutex;
	};
}

#endif

//=====

#if defined(STDX_USING_MUTEX) || defined(STDX_USING_ALL)

namespace stdx
{ 
	using namespace mutex; 
}

#endif
