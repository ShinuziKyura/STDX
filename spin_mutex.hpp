#ifndef SPIN_MUTEX_HPP
#define SPIN_MUTEX_HPP

#include <atomic>

namespace stdx
{	
	class spin_mutex final
	{
	public:
		spin_mutex() = default;
		spin_mutex(spin_mutex const &) = delete;
		spin_mutex & operator=(spin_mutex const &) = delete;
		spin_mutex(spin_mutex &&) = delete;
		spin_mutex & operator=(spin_mutex &&) = delete;
	
		void lock()
		{
			while (_lock.test_and_set(std::memory_order_relaxed));

			std::atomic_thread_fence(std::memory_order_acquire);
		}
		bool try_lock()
		{
			if (!_lock.test_and_set(std::memory_order_relaxed))
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				
				return true;
			}
			return false;
		}
		void unlock()
		{
			_lock.clear(std::memory_order_release);
		}
	private:
		std::atomic_flag _lock = ATOMIC_FLAG_INIT;
	};
}

#endif
