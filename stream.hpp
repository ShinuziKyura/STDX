#ifndef STDX_STREAM_HPP
#define STDX_STREAM_HPP

#include <iostream>
//#include <sstream>
//#include <fstream>
//#include <streambuf>

namespace stdx::stream
{
	inline void wait_for_newline()
	{
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}
}

#if defined(STDX_USING_STREAM) || defined(STDX_USING_ALL)
namespace stdx { using namespace stream; }
#endif

#endif
