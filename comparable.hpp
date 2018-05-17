#ifndef COMPARABLE_HPP
#define COMPARABLE_HPP

namespace stdx
{
	template <class Type>
	struct comparable
	{
		bool operator>(comparable<Type> const & that)
		{
			return static_cast<Type const &>(that) < static_cast<Type const &>(*this);
		}
		bool operator<=(comparable<Type> const & that)
		{
			return !(static_cast<Type const &>(that) < static_cast<Type const &>(*this));
		}
		bool operator>=(comparable<Type> const & that)
		{
			return !(static_cast<Type const &>(*this) < static_cast<Type const &>(that));
		}
		bool operator==(comparable<Type> const & that)
		{
			return (!(static_cast<Type const &>(*this) < static_cast<Type const &>(that)) &&
					!(static_cast<Type const &>(that) < static_cast<Type const &>(*this)));
		}
		bool operator!=(comparable<Type> const & that)
		{
			return (static_cast<Type const &>(*this) < static_cast<Type const &>(that) ||
					static_cast<Type const &>(that) < static_cast<Type const &>(*this));
		}
	};
}

#endif
