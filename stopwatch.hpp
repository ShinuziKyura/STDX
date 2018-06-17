#ifndef STDX_STOPWATCH_HPP
#define STDX_STOPWATCH_HPP

#include <list>
#include <chrono>

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
			if (_running)
			{
				_split();
				return _split_times.back();
			}
			return 0.0;
		}
		static double pause()
		{
			if (_running)
			{
				_split();
				_running = false;
				return _total_time;
			}
			return 0.0;
		}
		static void reset()
		{
			_split_times.clear();
			_total_time = 0.0;
			_running = false;
		}
		static std::list<double> const split_times()
		{
			return _split_times;
		}
		static double total_time()
		{
			return _total_time;
		}
	private:
		static void _split()
		{
			auto start_point = std::exchange(_split_point, std::chrono::steady_clock::now());
			_total_time += _split_times.emplace_back(std::chrono::duration<double>(_split_point - start_point).count());
		}

		thread_local static bool _running;
		thread_local static std::chrono::steady_clock::time_point _split_point;
		thread_local static std::list<double> _split_times;
		thread_local static double _total_time;
	};

	bool									stopwatch::_running = false;
	std::chrono::steady_clock::time_point	stopwatch::_split_point;
	std::list<double>						stopwatch::_split_times;
	double									stopwatch::_total_time = 0.0;
}

#endif
