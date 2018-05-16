#ifndef ZEROMEM_HPP
#define ZEROMEM_HPP

#include <type_traits>

namespace stdx
{
	template <class Type>
	inline auto zeromem(Type * obj) -> std::enable_if_t<std::is_standard_layout_v<Type>>
	{
		auto obj_ptr = reinterpret_cast<std::byte *>(obj);
		for (size_t idx = 0; idx < sizeof *obj; ++idx)
		{
			obj_ptr[idx] ^= obj_ptr[idx];
		}
	}
}

#endif//ZEROMEM_HPP
