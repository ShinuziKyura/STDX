#ifndef STDX_SPIN_MUTEX_HPP
#define STDX_SPIN_MUTEX_HPP

#include <atomic>

namespace stdx::mutex
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
}

#if defined(STDX_USING_MUTEX) || defined(STDX_USING_ALL)
namespace stdx { using namespace mutex; }
#endif

#endif
