#ifndef STDX_STOPWATCH_HPP
#define STDX_STOPWATCH_HPP

#include <list>
#include <chrono>

#ifndef STDX_STOPWATCH_RESOLUTION
#define STDX_STOPWATCH_RESOLUTION std::ratio<1>
#endif

namespace stdx // chrono
{
	class stopwatch
	{
	public:
		static void start()
		{
			if (!_counting)
			{
				_counting = true;
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
			if (!_counting)
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
		static bool counting()
		{
			return _counting;
		}
	private:
		static double _split(bool counting)
		{
			if (_counting)
			{
				auto start_point = std::exchange(_split_point, std::chrono::steady_clock::now());
				_total_time += _split_times.emplace_back(std::chrono::duration<double, STDX_STOPWATCH_RESOLUTION>(_split_point - start_point).count());
				_counting = counting;
				return _split_times.back();
			}
			return 0.0;
		}

		thread_local static bool _counting;
		thread_local static std::chrono::steady_clock::time_point _split_point;
		thread_local static std::list<double> _split_times;
		thread_local static double _total_time;
	};

	thread_local bool stopwatch::_counting = false;
	thread_local std::chrono::steady_clock::time_point stopwatch::_split_point;
	thread_local std::list<double> stopwatch::_split_times;
	thread_local double stopwatch::_total_time = 0.0;
}

#endif
