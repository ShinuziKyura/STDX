#ifndef STDX_TEMPLATES_HPP
#define STDX_TEMPLATES_HPP

#include <atomic>

namespace stdx::templates
{
	// Class template traits

		// Obtain class from template

	template <class>
	struct class_template;

	template <template <class ...> class Template, class ... Parameters>
	struct class_template<Template<Parameters ...>>
	{
		template <class ... Parameters1>
		using type = Template<Parameters1 ...>;
	};

		// Cast from one type to another

	template <bool, class>
	struct _type_cast
	{
	};

	template <class Type>
	struct _type_cast<true, Type>
	{
		using type = Type;
	};

	template <class Type1, class Type2>
	using type_cast = typename _type_cast<std::is_convertible_v<Type2, Type1>, Type1>::type;

		// Class-template equality

	template <class, template <class ...> class>
	struct is_same_as : std::false_type
	{
	};

	template <template <class ...> class Template1, class ... Parameters, template <class ...> class Template2>
	struct is_same_as<Template1<Parameters ...>, Template2> : std::is_same<Template1<Parameters ...>, Template2<Parameters ...>>
	{
	};

	template <class Template1, template <class ...> class Template2>
	inline constexpr bool is_same_as_v = is_same_as<Template1, Template2>::value;

		// Class-template equality, alternative for templates that take non-type parameters (not for external use)

	template <class, template <auto ...> class>
	struct _is_same_as : std::false_type
	{
	};

	template <template <auto ...> class Template1, auto ... Parameters, template <auto ...> class Template2>
	struct _is_same_as<Template1<Parameters ...>, Template2> : std::is_same<Template1<Parameters ...>, Template2<Parameters ...>>
	{
	};

	template <class Template1, template <auto ...> class Template2>
	inline constexpr bool _is_same_as_v = _is_same_as<Template1, Template2>::value;

		// Template-template equality

	template <template <class ...> class, template <class ...> class>
	struct is_same_with : std::false_type
	{
	};

	template <template <class ...> class Template>
	struct is_same_with<Template, Template> : std::true_type
	{
	};

	template <template <class ...> class Template1, template <class ...> class Template2>
	inline constexpr bool is_same_with_v = is_same_with<Template1, Template2>::value;

	// Numeric traits

		// Range

	template <size_t N, size_t Min, size_t Max>
	struct _between : std::bool_constant<bool(Min <= N && N <= Max)>
	{
	};

	template <size_t N, size_t Min, size_t Max>
	struct _beside : std::bool_constant<bool(N < Min || Max < N)>
	{
	};
	
	template <size_t Min, size_t Max>
	struct range
	{
		template <auto N>
		using between = _between<N, Min, Max>;
		template <auto N>
		using beside = _beside<N, Min, Max>;
	};

	// Atomic traits

		// Determines if built-in atomic types are lock-free, assuming that these are properly aligned

	template <class>
	struct is_lock_free;

	template <class Type>
	struct is_lock_free<std::atomic<Type>> : std::false_type // Review this (hint: determine by appropriate is_lock_free<std::atomic<integral> based on sizeof(Type))
	{
	};

#ifdef ATOMIC_BOOL_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<bool>> : std::bool_constant<bool(ATOMIC_BOOL_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_CHAR_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char>> : std::bool_constant<bool(ATOMIC_CHAR_LOCK_FREE)>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned char>> : std::bool_constant<bool(ATOMIC_CHAR_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_CHAR16_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char16_t>> : std::bool_constant<bool(ATOMIC_CHAR16_T_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_CHAR32_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char32_t>> : std::bool_constant<bool(ATOMIC_CHAR32_T_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_WCHAR_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<wchar_t>> : std::bool_constant<bool(ATOMIC_WCHAR_T_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_SHORT_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<short>> : std::bool_constant<bool(ATOMIC_SHORT_LOCK_FREE)>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned short>> : std::bool_constant<bool(ATOMIC_SHORT_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_INT_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<int>> : std::bool_constant<bool(ATOMIC_INT_LOCK_FREE)>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned int>> : std::bool_constant<bool(ATOMIC_INT_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_LONG_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<long>> : std::bool_constant<bool(ATOMIC_LONG_LOCK_FREE)>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned long>> : std::bool_constant<bool(ATOMIC_LONG_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_LLONG_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<long long>> : std::bool_constant<bool(ATOMIC_LLONG_LOCK_FREE)>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned long long>> : std::bool_constant<bool(ATOMIC_LLONG_LOCK_FREE)>
	{
	};
#endif

#ifdef ATOMIC_POINTER_LOCK_FREE
	template <class Type>
	struct is_lock_free<std::atomic<Type *>> : std::bool_constant<bool(ATOMIC_POINTER_LOCK_FREE)>
	{
	};
#endif

	// Pairs and tuples

		// Typepair

	template <class Type1, class Type2>
	struct typepair
	{
		using first = Type1;
		using second = Type2;
	};

		// Valuepair

	template <auto Value1, auto Value2>
	struct valuepair
	{
		static constexpr auto first = Value1;
		static constexpr auto second = Value2;
	};

		// Typevalue, a special pair with a type parameter as first parameter and a non-type parameter as second parameter

	template <class Type, auto Value>
	struct typevalue
	{
		using first = Type;
		static constexpr auto second = Value;
	};

		// Typetuple

	template <class ...>
	struct typetuple
	{
		static constexpr size_t size = 0;
		template <class ... Types>
		using push = typetuple<Types ...>;
		template <size_t N>
		using pop = typetuple<>;
	};

	template <class Type>
	struct typetuple<Type>
	{
		using first = Type;
		using last = Type;
		static constexpr size_t size = 1;
		template <class ... Types>
		using push = typetuple<Type, Types ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typetuple<>, typetuple<Type>>;
	};

	template <class Type, class ... Types>
	struct typetuple<Type, Types ...>
	{
		using first = Type;
		using last = typename typetuple<Types ...>::last;
		static constexpr size_t size = 1 + sizeof...(Types);
		template <class ... Types1>
		using push = typetuple<Type, Types ..., Types1 ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename typetuple<Types ...>::template pop<N - 1>, typetuple<Type, Types ...>>;
	};

		// Valuetuple

	template <auto ...>
	struct valuetuple
	{
		static constexpr size_t size = 0;
		template <auto ... Values>
		using push = valuetuple<Values ...>;
		template <size_t N>
		using pop = valuetuple<>;
	};

	template <auto Value>
	struct valuetuple<Value>
	{
		static constexpr auto first = Value;
		static constexpr auto last = Value;
		static constexpr size_t size = 1;
		template <auto ... Values>
		using push = valuetuple<Value, Values ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), valuetuple<>, valuetuple<Value>>;
	};

	template <auto Value, auto ... Values>
	struct valuetuple<Value, Values ...>
	{
		static constexpr auto first = Value;
		static constexpr auto last = valuetuple<Values ...>::last;
		static constexpr size_t size = 1 + sizeof...(Values);
		template <auto ... Values1>
		using push = valuetuple<Value, Values ..., Values1 ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename valuetuple<Values ...>::template pop<N - 1>, valuetuple<Value, Values ...>>;
	};

		// Constrained typetuple, applies a trait to each element of a typetuple (which can be of type typepair), constructing a new typetuple with the elements of the old one (or the second types of the typepairs) for which bool(trait<element>::value) == true (or bool(trait<element::first>::value) == true)

	template <template <class> class Trait, class, class Typetuple>
	struct _constrained_typetuple : _constrained_typetuple<Trait, Typetuple, typetuple<>>
	{
		static_assert(is_same_as_v<Typetuple, typetuple>,
					  "stdx::templates::constrained_typetuple<Trait, Typetuple>: Typetuple must be of type stdx::templates::typetuple<T ...>");
	};

	template <template <class> class Trait, class ... Types, class Typetuple>
	struct _constrained_typetuple<Trait, typetuple<Types ...>, Typetuple> : _constrained_typetuple<Trait, typename typetuple<Types ...>::template pop<1>, std::conditional_t<bool(Trait<typename typetuple<Types ...>::first>::value), typename Typetuple::template push<typename typetuple<Types ...>::first>, Typetuple>>
	{
	};

	template <template <class> class Trait, class Type1, class Type2, class ... Types, class Typetuple>
	struct _constrained_typetuple<Trait, typetuple<typepair<Type1, Type2>, Types ...>, Typetuple> : _constrained_typetuple<Trait, typetuple<Types ...>, std::conditional_t<bool(Trait<Type1>::value), typename Typetuple::template push<Type2>, Typetuple>>
	{
	};

	template <template <class> class Trait, class Typetuple>
	struct _constrained_typetuple<Trait, typetuple<>, Typetuple>
	{
		using type = Typetuple;
	};

	template <template <class> class Trait, class Typetuple>
	using constrained_typetuple = typename _constrained_typetuple<Trait, void, Typetuple>::type;

		// Constrained valuetuple, applies a trait to each element of a valuetuple, constructing a new valuetuple with the elements of the old one for which bool(trait<element>::value) == true


	template <template <auto> class Trait, class, class Valuetuple>
	struct _constrained_valuetuple : _constrained_valuetuple<Trait, Valuetuple, valuetuple<>>
	{
		static_assert(_is_same_as_v<Valuetuple, valuetuple>,
					  "stdx::templates::constrained_valuetuple<Trait, Valuetuple>: Typetuple must be of type stdx::templates::valuetuple<V ...>");
	};

	template <template <auto> class Trait, auto ... Values, class Valuetuple>
	struct _constrained_valuetuple<Trait, valuetuple<Values ...>, Valuetuple> : _constrained_valuetuple<Trait, typename valuetuple<Values ...>::template pop<1>, std::conditional_t<bool(Trait<valuetuple<Values ...>::first>::value), typename Valuetuple::template push<valuetuple<Values ...>::first>, Valuetuple>>
	{
	};

	template <template <auto> class Trait, class Valuetuple>
	struct _constrained_valuetuple<Trait, valuetuple<>, Valuetuple>
	{
		using type = Valuetuple;
	};

	template <template <auto> class Trait, class Valuetuple>
	using constrained_valuetuple = typename _constrained_valuetuple<Trait, void, Valuetuple>::type;

		// Constrained typevaluetuple, applies a trait to each element of a typetuple (which must be of type typevalue), constructing a valuetuple with the elements for which bool(trait<element::first>::value) == true

	template <template <class> class Trait, class, class Typetuple>
	struct _constrained_typevaluetuple : _constrained_typevaluetuple<Trait, Typetuple, valuetuple<>>
	{
		static_assert(is_same_as_v<Typetuple, typetuple>,
					  "stdx::templates::constrained_typevaluetuple<Trait, Typetuple>: Typetuple must be of type stdx::templates::typetuple<TV ...> where TV is of type stdx::templates::typevalue<T, V>");
	};

	template <template <class> class Trait, class Type, auto Value, class ... Types, class Valuetuple>
	struct _constrained_typevaluetuple<Trait, typetuple<typevalue<Type, Value>, Types ...>, Valuetuple> : _constrained_typevaluetuple<Trait, typetuple<Types ...>, std::conditional_t<bool(Trait<Type>::value), typename Valuetuple::template push<Value>, Valuetuple>>
	{
	};

	template <template <class> class Trait, class Valuetuple>
	struct _constrained_typevaluetuple<Trait, typetuple<>, Valuetuple>
	{
		using type = Valuetuple;
	};

	template <template <class> class Trait, class Typetuple>
	using constrained_typevaluetuple = typename _constrained_typevaluetuple<Trait, void, Typetuple>::type;

		// Reorder typetuple, reorders the parameters inside a typetuple based on a valuetuple with integral values in the interval [0, N) where N is the number of elements in the tuples

	template <class>
	struct _check_valuetuple : std::false_type
	{
	};

	template <auto ... Values>
	struct _check_valuetuple<valuetuple<Values ...>> : std::is_same<valuetuple<Values ...>, constrained_valuetuple<range<0, sizeof...(Values) - 1>::template between, constrained_typevaluetuple<std::is_integral, typetuple<typevalue<decltype(Values), Values> ...>>>>
	{
	};

	template <class Tuple, class Valuetuple, class>
	struct _reordered_tuple : _reordered_tuple<Tuple, typetuple<>, Valuetuple>
	{
		static_assert(is_same_as_v<Tuple, typetuple> || _is_same_as_v<Tuple, valuetuple>,
					  "stdx::templates::reordered_typetuple<Tuple, Valuetuple>: Tuple must be either of type stdx::templates::typetuple<T ...> or stdx::templates::valuetuple<V ...>");
		static_assert(_is_same_as_v<Valuetuple, valuetuple>,
					  "stdx::templates::reordered_typetuple<Tuple, Valuetuple>: Valuetuple must be of type stdx::templates::valuetuple<V ...>");
		static_assert(Tuple::size == Valuetuple::size,
					  "stdx::templates::reordered_typetuple<Tuple, Valuetuple>: Tuple::size must be equal to Valuetuple::size");
		static_assert(_check_valuetuple<Valuetuple>::value,
					  "stdx::templates::reordered_typetuple<Tuple, Valuetuple>: Valuetuple must contain only integral types and these must be in the interval of [0, Valuetuple::size)");
	};

	template <class Tuple1, class Tuple2, auto ... Values>
	struct _reordered_tuple<Tuple1, Tuple2, valuetuple<Values ...>> : _reordered_tuple<Tuple1, typename Tuple2::template push<typename Tuple1::template pop<valuetuple<Values ...>::first>::first>, typename valuetuple<Values ...>::template pop<1>>
	{
	};

	template <class Tuple1, class Tuple2>
	struct _reordered_tuple<Tuple1, Tuple2, valuetuple<>>
	{
		using type = Tuple2;
	};

	template <class Tuple, class Valuetuple>
	using reordered_tuple = typename _reordered_tuple<Tuple, Valuetuple, void>::type;
}

#endif
