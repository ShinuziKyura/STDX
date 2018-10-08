#ifndef STDX_STOPWATCH_HPP
#define STDX_STOPWATCH_HPP

#include <chrono>
#include <list>

#ifndef STDX_STOPWATCH_RESOLUTION
#define STDX_STOPWATCH_RESOLUTION std::ratio<1>
#endif

namespace stdx::chrono
{	
	class stopwatch
	{
	public:
		static void start()
		{
			if (!_is_ticking)
			{
				_is_ticking = true;
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
			if (!_is_ticking)
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
		static bool is_ticking()
		{
			return _is_ticking;
		}
	private:
		static double _split(bool ticking)
		{
			if (_is_ticking)
			{
				_is_ticking = ticking;

				auto start_point = std::exchange(_split_point, std::chrono::steady_clock::now());
				_total_time += _split_times.emplace_back(std::chrono::duration<double, STDX_STOPWATCH_RESOLUTION>(_split_point - start_point).count());
				return _split_times.back();
			}
			return 0.0;
		}
		// MSVC throws read access violation in list destructor while in debug mode if these are inline, need further investigation
#if defined(_MSC_VER) && defined(_DEBUG)
		thread_local static bool									_is_ticking;
		thread_local static std::chrono::steady_clock::time_point	_split_point;
		thread_local static std::list<double>						_split_times;
		thread_local static double									_total_time;
#else
		thread_local inline static bool										_is_ticking = false;
		thread_local inline static std::chrono::steady_clock::time_point	_split_point;
		thread_local inline static std::list<double>						_split_times;
		thread_local inline static double									_total_time = 0.0;
#endif
	};
}

#if defined(STDX_USING_CHRONO) || defined(STDX_USING_ALL)
namespace stdx { using namespace chrono; }
#endif

#endif
