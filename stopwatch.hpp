#ifndef STDX_header_STOPWATCH
#define STDX_header_STOPWATCH

#include <utility>
#include <deque>
#include <chrono>

// Defines stopwatch alias with std::chrono::duration<Rep, Period> as the stopwatch resolution. Defaults to stdx::chrono::seconds if no resolution is specified.
// Do not define this in a header file.
#define STDX_DEFINE_STOPWATCH(...) using stopwatch = ::stdx::chrono::_stopwatch<__VA_ARGS__>

namespace stdx::chrono
{	
	using nanoseconds = std::chrono::duration<long double, std::nano>;
	using microseconds = std::chrono::duration<long double, std::micro>;
	using milliseconds = std::chrono::duration<long double, std::milli>;
	using seconds = std::chrono::duration<long double>;
	using minutes = std::chrono::duration<long double, std::ratio<60>>;
	using hours = std::chrono::duration<long double, std::ratio<3600>>;

	template <class Resolution = seconds>
	class _stopwatch
	{
	public:
		using rep_type = typename Resolution::rep;
		using period_type = typename Resolution::period;

		static void start()
		{
			if (!_is_ticking)
			{
				_is_ticking = true;
				_split_point = std::chrono::steady_clock::now();
			}
		}
		static rep_type split()
		{
			return _split(true);
		}
		static rep_type stop()
		{
			return _split(false);
		}
		static void clear()
		{
			if (!_is_ticking)
			{
				_split_times.clear();
				_total_time = rep_type();
			}
		}
		static std::deque<rep_type> split_times()
		{
			return _split_times;
		}
		static rep_type total_time()
		{
			return _total_time;
		}
		static bool is_ticking()
		{
			return _is_ticking;
		}
	private:
		static rep_type _split(bool ticking)
		{
			if (_is_ticking)
			{
				auto const start_point = std::exchange(_split_point, std::chrono::steady_clock::now());
				auto split_time = _split_times.emplace_back(std::chrono::duration_cast<Resolution>(_split_point - start_point).count());
				
				_total_time += split_time;
				_is_ticking = ticking;

				return split_time;
			}
			return rep_type();
		}

		inline thread_local static bool										_is_ticking = false;
		inline thread_local static std::chrono::steady_clock::time_point	_split_point;
		inline thread_local static std::deque<rep_type>						_split_times;
		inline thread_local static rep_type									_total_time = rep_type();
	};
}

#endif
