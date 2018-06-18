#ifndef STDX_STOPWATCH_HPP
#define STDX_STOPWATCH_HPP

#include <list>
#include <chrono>

#ifndef STDX_STOPWATCH_RESOLUTION
#define STDX_STOPWATCH_RESOLUTION std::ratio<1>
#endif

namespace stdx
{
	class stopwatch
	{
	public:
		static void start()
		{
			if (!_running)
			{
				_running = true;
				_split_point = std::chrono::steady_clock::now();
			}
		}
		static double split()
		{
			return _split(true);
		}
		static double stop()
		{
			return _split(false);
		}
		static void clear()
		{
			if (!_running)
			{
				_split_times.clear();
				_total_time = 0.0;
			}
		}
		static std::list<double> split_times()
		{
			return _split_times;
		}
		static double total_time()
		{
			return _total_time;
		}
	private:
		static double _split(bool running)
		{
			if (_running)
			{
				auto start_point = std::exchange(_split_point, std::chrono::steady_clock::now());
				_total_time += _split_times.emplace_back(std::chrono::duration<double, STDX_STOPWATCH_RESOLUTION>(_split_point - start_point).count());
				_running = running;
				return _split_times.back();
			}
			return 0.0;
		}

		thread_local static bool _running;
		thread_local static std::chrono::steady_clock::time_point _split_point;
		thread_local static std::list<double> _split_times;
		thread_local static double _total_time;
	};

	thread_local bool stopwatch::_running = false;
	thread_local std::chrono::steady_clock::time_point stopwatch::_split_point;
	thread_local std::list<double> stopwatch::_split_times;
	thread_local double stopwatch::_total_time = 0.0;
}

#endif
