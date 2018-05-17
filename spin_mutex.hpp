#ifndef SPIN_MUTEX_HPP
#define SPIN_MUTEX_HPP

#include <thread>
#include <atomic>

namespace stdx
{	
	class spin_mutex
	{
	public:
		void lock()
		{
			while (_locked.test_and_set(std::memory_order_relaxed))
			{
				std::this_thread::yield();
			}

			std::atomic_thread_fence(std::memory_order_acquire);
			_owner = std::this_thread::get_id();
		}
		bool try_lock()
		{
			if (!_locked.test_and_set(std::memory_order_relaxed))
			{
				std::atomic_thread_fence(std::memory_order_acquire);
				_owner = std::this_thread::get_id();

				return true;
			}
			return false;
		}
		void unlock()
		{
			if (_owner == std::this_thread::get_id())
			{
				_owner = std::thread::id();
				_locked.clear(std::memory_order_release);
			}
		}

		spin_mutex() = default;
		spin_mutex(spin_mutex const &) = delete;
		spin_mutex & operator=(spin_mutex const &) = delete;
		~spin_mutex() = default;
	private:
		std::thread::id _owner;
		std::atomic_flag _locked{ ATOMIC_FLAG_INIT };
	};
}

#endif//SPIN_MUTEX_HPP
