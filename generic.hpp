#ifndef GENERIC_HPP
#define GENERIC_HPP

#include <type_traits>

namespace stdx
{	// WIP
	class generic /*
	{
	public:
		template <class Type>
		generic(Type && value) : _counter(new _container<Type>(value))
		{
		}
	private:
		struct _container_base
		{
			virtual ~_container_base() = default;
		};
		template <class Type>
		struct _container : _container_base
		{
			_container<Type>(Type && val) :
				value(std::forward<Type>(val))
			{
			}
			~_container<Type>() = default;

			Type value;
		};

		_container_base * _counter;
	}/**/;
}

#endif//GENERIC_HPP
