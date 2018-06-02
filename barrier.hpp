#ifndef BARRIER_HPP
#define BARRIER_HPP

#include <future>

namespace stdx
{
	class barrier // TODO
	{
	public:
		barrier() :
			_is_signal(true),
			_barrier(_signal.get_future().share()),
			_access_ref(_access)
		{
		}
		barrier(barrier const & other) :
			_is_signal(false),
			_barrier(other._shared_future()),
			_access_ref(other._access_ref)
		{
			_access_ref.clear(std::memory_order_release);
		}
		barrier & operator=(barrier const &) = delete;
		barrier(barrier && other) : // Exactly like copy ctor for now
			_is_signal(false),
			_barrier(other._shared_future()),
			_access_ref(other._access_ref)
		{
			_access_ref.clear(std::memory_order_release);
		}
		barrier & operator=(barrier &&) = delete;

		void signal()
		{
			if (_is_signal)
			{
				while (_access_ref.test_and_set(std::memory_order_acquire));

				_signal.set_value();

				_access_ref.clear(std::memory_order_relaxed);
			}
		}
		void wait()
		{
			while (_access_ref.test_and_set(std::memory_order_relaxed));

			auto barrier{ _barrier };

			_access_ref.clear(std::memory_order_release);

			barrier.wait();
		}
	private:
		std::shared_future<void> & _shared_future() const
		{
			while (_access_ref.test_and_set(std::memory_order_relaxed));

			return _barrier;
		}

		bool const _is_signal;
		std::promise<void> _signal;
		std::shared_future<void> mutable _barrier;
		std::atomic_flag mutable _access{ ATOMIC_FLAG_INIT };
		std::atomic_flag & _access_ref;
	};
}

#endif
