#ifndef STDX_ZEROMEM_HPP
#define STDX_ZEROMEM_HPP

#include <type_traits>

namespace stdx
{
	template <class Type>
	inline auto zeromem(Type & obj) -> std::enable_if_t<std::conjunction_v<std::is_standard_layout<Type>, std::negation<std::is_const<Type>>>>
	{
		memset(&obj, 0, sizeof obj);
	}
}

#endif
