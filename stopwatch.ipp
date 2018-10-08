#if defined(_MSC_VER) && defined(_DEBUG)
#include "stopwatch.hpp"

// Consider this as a compilation unit
thread_local bool									stdx::chrono::stopwatch::_is_ticking = false;
thread_local std::chrono::steady_clock::time_point	stdx::chrono::stopwatch::_split_point;
thread_local std::list<double>						stdx::chrono::stopwatch::_split_times;
thread_local double									stdx::chrono::stopwatch::_total_time = 0.0;
#endif