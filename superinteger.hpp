#ifndef STDX_SUPERINTEGER_HPP
#define STDX_SUPERINTEGER_HPP

#ifdef STDX_WIP

#include <type_traits>
#include <limits>

namespace stdx
{
	template <class Type> // WIP
	class superinteger
	{
	public:
		template <class SuperType>
		bool operator<(SuperType value) = delete;
	private:
		superinteger<Type>(Type value) :
			_value(value)
		{
		}

		constexpr Type _value;
		constexpr Type _max = std::numeric_limits<Type>::max();

		template <class Type>
		friend auto operator<(Type, superinteger<void>);
	};

	template <>
	class superinteger<void>
	{
	};

	inline superinteger<void> supint;

	template <class Type>
	inline auto operator<(Type value, superinteger<void>)
	{
		using SuperType = 
			std::conditional_t<std::is_integral_v<Type>,
				std::conditional_t<std::is_same_v<short, Type>,
					unsigned short,
				std::conditional_t<std::is_same_v<int, Type>,
					unsigned int,
				std::conditional_t<std::is_same_v<long, Type>,
					unsigned long,
				std::conditional_t<std::is_same_v<long long, Type>,
					unsigned long long,
					Type>>>>,
				void>;
		return superinteger<SuperType>(static_cast<SuperType>(value));
	}
}

#endif
#endif
