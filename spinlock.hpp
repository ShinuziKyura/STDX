#ifndef STDX_SPINLOCK_HPP
#define STDX_SPINLOCK_HPP

#include <limits>
#include <thread>
#include <atomic>

#include "meta.hpp"

namespace stdx::thread
{	
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
			while (_lock.test_and_set(std::memory_order_acquire))
			{
				std::this_thread::yield();
			}
		}
		bool try_lock()
		{
			return !_lock.test_and_set(std::memory_order_acquire);
		}
		void unlock()
		{
			_lock.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag _lock = ATOMIC_FLAG_INIT;
	};

	class spin_shared_mutex
	{
		using _atomic_int_largest_lock_free_t =
			typename stdx::meta::constrained_pack<
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
			typename _atomic_int_largest_lock_free_t::value_type;
	public:
		spin_shared_mutex() noexcept = default;
		spin_shared_mutex(spin_shared_mutex const &) = delete;
		spin_shared_mutex & operator=(spin_shared_mutex const &) = delete;
		spin_shared_mutex(spin_shared_mutex &&) = delete;
		spin_shared_mutex & operator=(spin_shared_mutex &&) = delete;

		void lock()
		{
			_int_largest_lock_free_t available = 0;

			while (!_lock.compare_exchange_weak(available, std::numeric_limits<_int_largest_lock_free_t>::min(), std::memory_order_acquire))
			{
				std::this_thread::yield();
				available = 0;
			}
		}
		bool try_lock()
		{
			_int_largest_lock_free_t available = 0;

			return _lock.compare_exchange_weak(available, std::numeric_limits<_int_largest_lock_free_t>::min(), std::memory_order_acquire);
		}
		void unlock()
		{
			_lock.store(0, std::memory_order_release);
		}
		void lock_shared()
		{
			_int_largest_lock_free_t available = _lock.load(std::memory_order_relaxed);

			do
			{
				switch (available)
				{
					case std::numeric_limits<_int_largest_lock_free_t>::min():
						std::this_thread::yield();
						available = 0;
						break;
					case std::numeric_limits<_int_largest_lock_free_t>::max():
						std::this_thread::yield();
						--available;
						break;
				}
			}
			while (!_lock.compare_exchange_weak(available, available + 1, std::memory_order_acquire));
		}
		bool try_lock_shared()
		{
			_int_largest_lock_free_t available = _lock.load(std::memory_order_relaxed);

			switch (available)
			{
				case std::numeric_limits<_int_largest_lock_free_t>::min():
					available = 0;
					break;
				case std::numeric_limits<_int_largest_lock_free_t>::max():
					--available;
					break;
			}

			return _lock.compare_exchange_weak(available, available + 1, std::memory_order_acquire);
		}
		void unlock_shared()
		{
			_lock.fetch_sub(1, std::memory_order_release);
		}

		static constexpr bool is_lock_free()
		{
			return stdx::meta::is_lock_free<_atomic_int_largest_lock_free_t>::value;
		}
		static constexpr auto maximum_number_of_owners()
		{
			return std::numeric_limits<_int_largest_lock_free_t>::max();
		}
	private:
		_atomic_int_largest_lock_free_t _lock = 0;
	};
}

#if defined(STDX_USING_THREAD) || defined(STDX_USING_ALL)
namespace stdx { using namespace thread; }
#endif

#endif
