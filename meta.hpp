#ifndef STDX_header_META
#define STDX_header_META

#include <type_traits>
#include <utility>
#include <complex>
#include <atomic>

// SFINAE is weird: https://stackoverflow.com/questions/39510630/member-function-template-selection-and-sfinae

namespace stdx::meta
{
	namespace _implementation
	{
		struct _undefined;

		template <std::size_t>
		struct _placeholder;
	}

	namespace qualifiers
	{
		struct _const;
		struct _volatile;
		struct _lvalue_reference;
		struct _rvalue_reference;
	}
	
	namespace specifiers
	{
		struct _noexcept;
	}

	namespace placeholders
	{
		using _1 = _implementation::_placeholder<1>;
		using _2 = _implementation::_placeholder<2>;
		using _3 = _implementation::_placeholder<3>;
		using _4 = _implementation::_placeholder<4>;
		using _5 = _implementation::_placeholder<5>;
		using _6 = _implementation::_placeholder<6>;
		using _7 = _implementation::_placeholder<7>;
		using _8 = _implementation::_placeholder<8>;
		using _9 = _implementation::_placeholder<9>;
		using _10 = _implementation::_placeholder<10>;
		using _11 = _implementation::_placeholder<11>;
		using _12 = _implementation::_placeholder<12>;
		using _13 = _implementation::_placeholder<13>;
		using _14 = _implementation::_placeholder<14>;
		using _15 = _implementation::_placeholder<15>;
		using _16 = _implementation::_placeholder<16>;
		using _17 = _implementation::_placeholder<17>;
		using _18 = _implementation::_placeholder<18>;
		using _19 = _implementation::_placeholder<19>;
		using _20 = _implementation::_placeholder<20>;
	}

	namespace literals
	{
		constexpr auto operator "" _constexpr_string(const char * string, std::size_t)
		{
			return string;
		}
		constexpr auto operator "" _constexpr_string(const wchar_t * string, std::size_t)
		{
			return string;
		}
#if _HAS_CXX2A
		constexpr auto operator "" _constexpr_string(const char8_t * string, std::size_t)
		{
			return string;
		}
#endif
		constexpr auto operator "" _constexpr_string(const char16_t * string, std::size_t)
		{
			return string;
		}
		constexpr auto operator "" _constexpr_string(const char32_t * string, std::size_t)
		{
			return string;
		}
	}
	
		// Type_identity trait, provides type alias for the type with which it was instantiated (remove in C++2a)

	template <class Type>
	struct type_identity
	{
		using type = Type;
	};

	// Container types

		// Vals

	template <auto Value>
	struct val
	{
		using type = decltype(Value);
		static constexpr auto value = Value;
	};

		// Packs

	template <class ...>
	struct pack
	{
		static constexpr std::size_t size = 0;

		template <class ... Types>
		using push = pack<Types ...>;
		template <std::size_t N>
		using pop = pack<>;
	};

	template <class Type>
	struct pack<Type>
	{
		static constexpr std::size_t size = 1;
		using first = Type;
		using last = Type;

		template <class ... Types>
		using push = pack<Type, Types ...>;
		template <std::size_t N>
		using pop = std::conditional_t<bool(N), pack<>, pack<Type>>;
	};

	template <class Type, class ... Types>
	struct pack<Type, Types ...>
	{
		static constexpr std::size_t size = 1 + sizeof...(Types);
		using first = Type;
		using last = typename pack<Types ...>::last;

		template <class ... Types1>
		using push = pack<Type, Types ..., Types1 ...>;
		template <std::size_t N>
		using pop = std::conditional_t<bool(N), typename pack<Types ...>::template pop<N - 1>, pack<Type, Types ...>>;
	};

		// Valpacks

	template <auto ...>
	struct valpack
	{
		static constexpr std::size_t size = 0;

		template <auto ... Values>
		using push = valpack<Values ...>;
		template <std::size_t N>
		using pop = valpack<>;
	};

	template <auto Value>
	struct valpack<Value>
	{
		static constexpr std::size_t size = 1;
		static constexpr auto first = Value;
		static constexpr auto last = Value;

		template <auto ... Values>
		using push = valpack<Value, Values ...>;
		template <std::size_t N>
		using pop = std::conditional_t<bool(N), valpack<>, valpack<Value>>;
	};

	template <auto Value, auto ... Values>
	struct valpack<Value, Values ...>
	{
		static constexpr std::size_t size = 1 + sizeof...(Values);
		static constexpr auto first = Value;
		static constexpr auto last = valpack<Values ...>::last;

		template <auto ... Values1>
		using push = valpack<Value, Values ..., Values1 ...>;
		template <std::size_t N>
		using pop = std::conditional_t<bool(N), typename valpack<Values ...>::template pop<N - 1>, valpack<Value, Values ...>>;
	};

	// Container modifiers

		// Convert from Pack of Vals to Valpack

	template <class>
	struct _as_valpack;

	template <auto ... Values>
	struct _as_valpack<valpack<Values ...>>
	{
		using _type = valpack<Values ...>;
	};

	template <auto ... Values>
	struct _as_valpack<pack<val<Values> ...>>
	{
		using _type = valpack<Values ...>;
	};

	template <>
	struct _as_valpack<pack<>>
	{
		using _type = valpack<>;
	};

	template <class Pack>
	using as_valpack = typename _as_valpack<Pack>::_type;

		// Convert from Valpack to Pack of Vals

	template <class>
	struct _as_pack_val;

	template <auto ... Values>
	struct _as_pack_val<pack<val<Values> ...>>
	{
		using _type = pack<val<Values> ...>;
	};

	template <>
	struct _as_pack_val<pack<>>
	{
		using _type = pack<>;
	};

	template <auto ... Values>
	struct _as_pack_val<valpack<Values ...>>
	{
		using _type = pack<val<Values> ...>;
	};

	template <class Valpack>
	using as_pack_val = typename _as_pack_val<Valpack>::_type;

		// Reverse pack elements

	template <class, class>
	struct _as_reverse_pack;

	template <class Pack, class ... ReverseTypes>
	struct _as_reverse_pack<Pack, pack<ReverseTypes ...>> : _as_reverse_pack<typename Pack::template pop<1>, pack<typename Pack::first, ReverseTypes ...>>
	{
	};

	template <class ReversePack>
	struct _as_reverse_pack<pack<>, ReversePack>
	{
		using _type = ReversePack;
	};

	template <class Pack>
	using as_reverse_pack = typename _as_reverse_pack<Pack, pack<>>::_type;

		// Reverse valpack elements

	template <class, class>
	struct _as_reverse_valpack;

	template <class Valpack, auto ... ReverseValues>
	struct _as_reverse_valpack<Valpack, valpack<ReverseValues ...>> : _as_reverse_valpack<typename Valpack::template pop<1>, valpack<Valpack::first, ReverseValues ...>>
	{
	};

	template <auto ... ReverseValues>
	struct _as_reverse_valpack<valpack<>, valpack<ReverseValues ...>>
	{
		using _type = valpack<ReverseValues ...>;
	};

	template <class Valpack>
	using as_reverse_valpack = typename _as_reverse_valpack<Valpack, valpack<>>::_type;

		// Apply a trait that takes a type parameter to a type from a Pack

	template <template <class> class Trait, std::size_t Index = 0>
	struct apply_to_pack_element
	{
		template <class Pack>
		using first = Trait<typename Pack::first>;
		template <class Pack>
		using nth = Trait<typename Pack::template pop<Index>::first>;
		template <class Pack>
		using last = Trait<typename Pack::last>;
	};

		// Apply a trait that takes a non-type parameter to a value from a Valpack

	template <template <auto> class Trait, std::size_t Index = 0>
	struct apply_to_valpack_element
	{
		template <class Valpack>
		using first = Trait<Valpack::first>;
		template <class Valpack>
		using nth = Trait<Valpack::template pop<Index>::first>;
		template <class Valpack>
		using last = Trait<Valpack::last>;
	};

	// Type traits

		// Check if a type is an instantiation of a class template

	template <class, template <class ...> class>
	struct is_template_instantiation : std::false_type
	{
	};

	template <template <class ...> class Template, class ... Parameters>
	struct is_template_instantiation<Template<Parameters ...>, Template> : std::true_type
	{
	};

	template <class Class, template <class ...> class Template>
	constexpr bool is_template_instantiation_v = is_template_instantiation<Class, Template>::value;

		// Apply a trait that takes a type parameter to a type alias that is a member of the instantiated type

	template <template <class> class Trait>
	struct apply_to_type
	{
		template <class Type>
		using trait = Trait<typename Type::type>;
	};

		// Apply a trait that takes a non-type parameter to a constant value that is a member of the instantiated type

	template <template <auto> class Trait>
	struct apply_to_value
	{
		template <class Type>
		using trait = Trait<Type::value>;
	};

		// Add cv through ref, provides type alias for Type, where if Type is a reference to a possibly cv-qualified type T, the type alias is of reference to that type T with added cv-qualifiers

	template <class Type>
	struct add_const_through_ref
	{
		using type = Type;
	};

	template <class Type>
	struct add_const_through_ref<Type &>
	{
		using type = Type const &;
	};

	template <class Type>
	struct add_const_through_ref<Type &&>
	{
		using type = Type const &&;
	};

	template <class Type>
	using add_const_through_ref_t = typename add_const_through_ref<Type>::type;

	template <class Type>
	struct add_volatile_through_ref
	{
		using type = Type;
	};

	template <class Type>
	struct add_volatile_through_ref<Type &>
	{
		using type = Type volatile &;
	};

	template <class Type>
	struct add_volatile_through_ref<Type &&>
	{
		using type = Type volatile &&;
	};

	template <class Type>
	using add_volatile_through_ref_t = typename add_volatile_through_ref<Type>::type;

	template <class Type>
	struct add_cv_through_ref
	{
		using type = typename add_const_through_ref<typename add_volatile_through_ref<Type>::type>::type;
	};

	template <class Type>
	using add_cv_through_ref_t = typename add_cv_through_ref<Type>::type;

		// Remove cv through ref, provides type alias for Type, where if Type is a reference to a possibly cv-qualified type T, the type alias is of reference to that type T with its cv-qualifiers removed

	template <class Type>
	struct remove_const_through_ref
	{
		using type = Type;
	};

	template <class Type>
	struct remove_const_through_ref<Type const &>
	{
		using type = Type &;
	};

	template <class Type>
	struct remove_const_through_ref<Type const &&>
	{
		using type = Type &&;
	};

	template <class Type>
	using remove_const_through_ref_t = typename remove_const_through_ref<Type>::type;

	template <class Type>
	struct remove_volatile_through_ref
	{
		using type = Type;
	};

	template <class Type>
	struct remove_volatile_through_ref<Type volatile &>
	{
		using type = Type &;
	};

	template <class Type>
	struct remove_volatile_through_ref<Type volatile &&>
	{
		using type = Type &&;
	};

	template <class Type>
	using remove_volatile_through_ref_t = typename remove_volatile_through_ref<Type>::type;

	template <class Type>
	struct remove_cv_through_ref
	{
		using type = typename remove_const_through_ref<typename remove_volatile_through_ref<Type>::type>::type;
	};

	template <class Type>
	using remove_cv_through_ref_t = typename remove_cv_through_ref<Type>::type;

	// Logic traits

		// Type if statement, end_if is an alias of the first type for which corresponding condition is true, or non-defined if all are false

	template <bool, class>
	struct _type_if;

	template <class Type>
	struct _type_end_if
	{
		using end_if = Type;
	};

	template <class Type>
	struct _type_then
	{
		template <bool>
		using else_if = _type_if<false, Type>;

		template <class>
		using else_then = _type_end_if<Type>;

		using end_if = Type;
	};

	template <>
	struct _type_then<_implementation::_undefined>
	{
		template <bool Condition>
		using else_if = _type_if<Condition, _implementation::_undefined>;

		template <class Type>
		using else_then = _type_end_if<Type>;
	};

	template <bool, class Type>
	struct _type_if
	{
		template <class>
		using then = _type_then<Type>;
	};

	template <>
	struct _type_if<false, _implementation::_undefined>
	{
		template <class Type>
		using then = _type_then<_implementation::_undefined>;
	};

	template <>
	struct _type_if<true, _implementation::_undefined>
	{
		template <class Type>
		using then = _type_then<Type>;
	};

	template <bool Condition>
	using type_if = _type_if<Condition, _implementation::_undefined>;
	
		// Value if statement, end_if is an alias of the first value for which corresponding condition is true, or non-defined if all are false

	template <bool, class>
	struct _value_if;

	template <class Val>
	struct _value_end_if
	{
		static constexpr auto end_if = Val::value;
	};

	template <class Val>
	struct _value_then
	{
		template <bool>
		using else_if = _value_if<false, Val>;

		template <auto>
		using else_then = _value_end_if<Val>;

		static constexpr auto end_if = Val::value;
	};

	template <>
	struct _value_then<_implementation::_undefined>
	{
		template <bool Condition>
		using else_if = _value_if<Condition, _implementation::_undefined>;

		template <auto Value>
		using else_then = _value_end_if<val<Value>>;
	};

	template <bool, class Val>
	struct _value_if
	{
		template <auto>
		using then = _value_then<Val>;
	};

	template <>
	struct _value_if<false, _implementation::_undefined>
	{
		template <auto Value>
		using then = _value_then<_implementation::_undefined>;
	};

	template <>
	struct _value_if<true, _implementation::_undefined>
	{
		template <auto Value>
		using then = _value_then<val<Value>>;
	};

	template <bool Condition>
	using value_if = _value_if<Condition, _implementation::_undefined>;

		// Conjunction

	template <template <class> class Trait, class ... Types>
	struct conjunction : std::bool_constant<(... && Trait<Types>::value)>
	{
	};

	template <template <class> class Trait, class ... Types>
	constexpr bool conjunction_v = conjunction<Trait, Types ...>::value;

		// Disjunction

	template <template <class> class Trait, class ... Types>
	struct disjunction : std::bool_constant<(... || Trait<Types>::value)>
	{
	};

	template <template <class> class Trait, class ... Types>
	constexpr bool disjunction_v = disjunction<Trait, Types ...>::value;

	// Numeric traits

		// Aliases

	using ssize_t = 
		type_if<bool(sizeof(std::size_t) == sizeof(unsigned long long))>::then<
			long long
		>::else_if<bool(sizeof(std::size_t) == sizeof(unsigned long))>::then<
			long
		>::else_if<bool(sizeof(std::size_t) == sizeof(unsigned int))>::then<
			int
		>::else_if<bool(sizeof(std::size_t) == sizeof(unsigned short))>::then<
			short
		>::end_if;

	template <class ArithmeticType>
	using half_t = 
		typename type_if<std::is_unsigned_v<ArithmeticType>>::template then<
			typename type_if<bool(sizeof(ArithmeticType) == sizeof(unsigned long long))>::template then<
				type_if<bool(sizeof(unsigned long) < sizeof(unsigned long long))>::then<
					unsigned long
				>::else_if<bool(sizeof(unsigned int) < sizeof(unsigned long long))>::then<
					unsigned int
				>::else_if<bool(sizeof(unsigned short) < sizeof(unsigned long long))>::then<
					unsigned short
				>::else_if<bool(sizeof(unsigned char) < sizeof(unsigned long long))>::then<
					unsigned char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(unsigned long))>::template then<
				type_if<bool(sizeof(unsigned int) < sizeof(unsigned long))>::then<
					unsigned int
				>::else_if<bool(sizeof(unsigned short) < sizeof(unsigned long))>::then<
					unsigned short
				>::else_if<bool(sizeof(unsigned char) < sizeof(unsigned long))>::then<
					unsigned char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(unsigned int))>::template then<
				type_if<bool(sizeof(unsigned short) < sizeof(unsigned int))>::then<
					unsigned short
				>::else_if<bool(sizeof(unsigned char) < sizeof(unsigned int))>::then<
					unsigned char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(unsigned short))>::template then<
				type_if<bool(sizeof(unsigned char) < sizeof(unsigned short))>::then<
					unsigned char
				>::end_if
			>::end_if
		>::template else_if<std::is_integral_v<ArithmeticType>>::template then<
			typename type_if<bool(sizeof(ArithmeticType) == sizeof(long long))>::template then<
				type_if<bool(sizeof(long) < sizeof(long long))>::then<
					long
				>::else_if<bool(sizeof(int) < sizeof(long long))>::then<
					int
				>::else_if<bool(sizeof(short) < sizeof(long long))>::then<
					short
				>::else_if<bool(sizeof(char) < sizeof(long long))>::then<
					char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(long))>::template then<
				type_if<bool(sizeof(int) < sizeof(long))>::then<
					int
				>::else_if<bool(sizeof(short) < sizeof(long))>::then<
					short
				>::else_if<bool(sizeof(char) < sizeof(long))>::then<
					char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(int))>::template then<
				type_if<bool(sizeof(short) < sizeof(int))>::then<
					short
				>::else_if<bool(sizeof(char) < sizeof(int))>::then<
					char
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(short))>::template then<
				type_if<bool(sizeof(char) < sizeof(short))>::then<
					char
				>::end_if
			>::end_if
		>::template else_if<std::is_floating_point_v<ArithmeticType>>::template then<
			typename type_if<bool(sizeof(ArithmeticType) == sizeof(long double))>::template then<
				type_if<bool(sizeof(double) < sizeof(long double))>::then<
					double
				>::else_if<bool(sizeof(float) < sizeof(long double))>::then<
					float
				>::end_if
			>::template else_if<bool(sizeof(ArithmeticType) == sizeof(double))>::template then<
				type_if<bool(sizeof(float) < sizeof(double))>::then<
					float
				>::end_if
			>::end_if
		>::end_if;

		// Boolean checks

	template <class Type>
	constexpr bool always_false = false;

	template <class Type>
	struct is_complex : std::false_type
	{
	};

	template <class Type>
	struct is_complex<std::complex<Type>> : std::true_type
	{
	};

	template <class Type>
	constexpr bool is_complex_v = is_complex<Type>::value;

		// Range

	template <auto Min, decltype(Min) Max>
	struct inside
	{
		template <decltype(Min) N>
		using exclusive = std::bool_constant<Min < N && N < Max>;
		template <decltype(Min) N>
		using inclusive = std::bool_constant<Min <= N && N <= Max>;
	};

	template <auto Min, decltype(Min) Max>
	struct outside
	{
		template <decltype(Min) N>
		using exclusive = std::bool_constant<N < Min || Max < N>;
		template <decltype(Min) N>
		using inclusive = std::bool_constant<N <= Min || Max <= N>;
	};

	template <class, auto>
	struct _make_integer_sequence;

	template <class Type, Type ... Elements, Type Offset>
	struct _make_integer_sequence<std::integer_sequence<Type, Elements ...>, Offset>
	{
		using _type = std::integer_sequence<Type, (Elements + Offset) ...>;
	};

	template <class Type, Type Min, Type Max>
	using make_integer_sequence = typename _make_integer_sequence<std::make_integer_sequence<Type, Max - Min + 1>, Min>::_type;

	template <std::size_t Min, std::size_t Max>
	using make_index_sequence = make_integer_sequence<std::size_t, Min, Max>;

		// Conversions

	template <char Char>
	struct _intchar
	{
		static_assert(Char >= '0' && Char <= '9', "Invalid character in number representation!");

		static constexpr std::size_t _value = Char - '0';
	};

	template <char Char>
	constexpr std::size_t intchar = _intchar<Char>::_value;

	template <class CharPack, std::size_t Position>
	struct _intstring
	{
		static constexpr std::size_t _value = _intstring<typename CharPack::template pop<1>, Position * 10>::_value + _intchar<CharPack::first>::_value * Position;
	};

	template <std::size_t Position>
	struct _intstring<valpack<>, Position>
	{
		static constexpr std::size_t _value = 0;
	};

	template <char ... Chars>
	constexpr std::size_t intstring = _intstring<as_reverse_valpack<valpack<Chars ...>>, 1>::_value;

	// Container types

		// Constrained pack, applies a trait to each element of a pack, constructing a new pack with the elements of the old one for which bool(trait<element>::value) == true

	template <template <class> class Trait, class InPack, class OutPack>
	struct _constrained_pack : _constrained_pack<Trait, typename InPack::template pop<1>, std::conditional_t<bool(Trait<typename InPack::first>::value), typename OutPack::template push<typename InPack::first>, OutPack>>
	{
	};

	template <template <class> class Trait, class OutPack>
	struct _constrained_pack<Trait, pack<>, OutPack>
	{
		using _type = OutPack;
	};

	template <template <class> class Trait, class InPack>
	struct _assert_constrained_pack : _constrained_pack<Trait, InPack, pack<>>
	{
		static_assert(is_template_instantiation_v<InPack, pack>,
					  "'stdx::meta::constrained_pack<Trait, InPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T are any type parameters");
	};

	template <template <class> class Trait, class InPack>
	using constrained_pack = typename _assert_constrained_pack<Trait, InPack>::_type;

		// Transformed pack, applies a trait to each element of a pack, constructing a new pack of the same size as the old one and with the corresponding elements as trait<element>::type

	template <template <class> class Trait, class InPack, class OutPack>
	struct _transformed_pack : _transformed_pack<Trait, typename InPack::template pop<1>, typename OutPack::template push<typename Trait<typename InPack::first>::type>>
	{
	};

	template <template <class> class Trait, class OutPack>
	struct _transformed_pack<Trait, pack<>, OutPack>
	{
		using _type = OutPack;
	};

	template <template <class> class Trait, class InPack>
	struct _assert_transformed_pack : _transformed_pack<Trait, InPack, pack<>>
	{
		static_assert(is_template_instantiation_v<InPack, pack>,
					  "'stdx::meta::transformed_pack<Trait, InPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T are any type parameters");
	};

	template <template <class> class Trait, class InPack>
	using transformed_pack = typename _assert_transformed_pack<Trait, InPack>::_type;

		// Permutated pack, permutates the elements of a pack based on a valpack with integral values in the interval [0, N) where N is the number of elements in the pack

	template <class InPack, class OutPack, class IndexPack>
	struct _permutated_pack : _permutated_pack<InPack, typename OutPack::template push<typename InPack::template pop<IndexPack::first>::first>, typename IndexPack::template pop<1>>
	{
	};

	template <class InPack, class OutPack>
	struct _permutated_pack<InPack, OutPack, valpack<>>
	{
		using _type = OutPack;
	};

	template <class IndexPack>
	struct _assert_index_values_permutated_pack : std::is_same<IndexPack, constrained_pack<apply_to_value<inside<0, IndexPack::size - 1>::template inclusive>::template trait, constrained_pack<apply_to_type<std::is_integral>::template trait, IndexPack>>>
	{
	};

	template <class InPack, class IndexPack>
	struct _assert_permutated_pack : _permutated_pack<InPack, pack<>, as_valpack<IndexPack>>
	{
		static_assert(is_template_instantiation_v<InPack, pack>,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T is any type parameter");
		static_assert(InPack::size == IndexPack::size,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "InPack::size must be equal to IndexPack::size");
		static_assert(_assert_index_values_permutated_pack<as_pack_val<IndexPack>>::value,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "IndexPack must be of type stdx::template::valpack<V ...> where V are unique integral value parameters in the interval of [0, IndexPack::size)");
	};

	template <class InPack, class IndexPack>
	using permutated_pack = typename _assert_permutated_pack<InPack, IndexPack>::_type;

		// Merged pack, merges several packs of the same size into one pack of packs where the nth pack contains the nth elements of each original pack

	template <std::size_t N, class OutPack, class ... InPack>
	struct _transposed_pack : _transposed_pack<N - 1, typename OutPack::template push<pack<typename InPack::first ...>>, typename InPack::template pop<1> ...>
	{
	};

	template <class OutPack, class ... InPack>
	struct _transposed_pack<0, OutPack, InPack ...>
	{
		using _type = OutPack;
	};

	template <class SizeType, class ... SizeTypes>
	constexpr bool _assert_pack_sizes_transposed_pack(SizeType size, SizeTypes ... sizes)
	{
		return (... && (size == sizes));
	};

	template <class ... InPack>
	struct _assert_transposed_pack : _transposed_pack<pack<InPack ...>::first::size, pack<>, InPack ...>
	{
		static_assert(_assert_pack_sizes_transposed_pack(InPack::size ...),
					  "'stdx::meta::transposed_pack<InPack ...>': "
					  "InPack::size must be equal for all packs");
	};

	template <class ... InPack>
	using transposed_pack = typename _assert_transposed_pack<InPack ...>::_type;

	// Functional traits

		// Function signature

	template <class, class ...>
	struct _function_signature_base;

	template <class RetType, class ... ParamTypes, class ... QSTypes>
	struct _function_signature_base<RetType(ParamTypes ...), QSTypes ...>
	{
		// Return type
		using return_type = RetType;
		// Parameter types
		using parameter_types = pack<ParamTypes ...>;
		// Qualifier / Specifier types
		using qs_types = pack<QSTypes ...>;
	};

	template <class>
	struct _function_signature;

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...)> : _function_signature_base<RetType(ParamTypes ...)>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) &> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_lvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const &> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_lvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile &> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile, qualifiers::_lvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile &> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile, qualifiers::_lvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) &&> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_rvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const &&> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_rvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile &&> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile, qualifiers::_rvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile &&> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile, qualifiers::_rvalue_reference>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) noexcept> : _function_signature_base<RetType(ParamTypes ...), specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) & noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_lvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const & noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_lvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile & noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile, qualifiers::_lvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile & noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile, qualifiers::_lvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) && noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_rvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const && noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_rvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) volatile && noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_volatile, qualifiers::_rvalue_reference, specifiers::_noexcept>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...) const volatile && noexcept> : _function_signature_base<RetType(ParamTypes ...), qualifiers::_const, qualifiers::_volatile, qualifiers::_rvalue_reference, specifiers::_noexcept>
	{
	};

	template <class FuncType>
	struct function_signature : _function_signature<std::remove_pointer_t<std::remove_reference_t<FuncType>>>
	{
		static_assert(std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>, ""); // TODO

		using object_type = void;
	};

	template <class FuncType, class ObjType>
	struct function_signature<FuncType ObjType::*> : _function_signature<std::remove_pointer_t<std::remove_reference_t<FuncType>>>
	{
		static_assert(std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>, ""); // TODO

		using object_type = ObjType;
	};

		// Make function signature

	template <class, class, class>
	struct _make_function_signature;

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<>>
	{
		using _type = RetType(ParamTypes ...);
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const>>
	{
		using _type = RetType(ParamTypes ...) const;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile>>
	{
		using _type = RetType(ParamTypes ...) volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile>>
	{
		using _type = RetType(ParamTypes ...) const volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_lvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_lvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) const &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile, qualifiers::_lvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile, qualifiers::_lvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) const volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_rvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) && ;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_rvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) const &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile, qualifiers::_rvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile, qualifiers::_rvalue_reference>>
	{
		using _type = RetType(ParamTypes ...) const volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_lvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_lvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile, qualifiers::_lvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile, qualifiers::_lvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_rvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_rvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_volatile, qualifiers::_rvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) volatile && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<RetType, pack<ParamTypes ...>, pack<qualifiers::_const, qualifiers::_volatile, qualifiers::_rvalue_reference, specifiers::_noexcept>>
	{
		using _type = RetType(ParamTypes ...) const volatile && noexcept;
	};

	template <class RetType, class ParamPackType, class QualType>
	using make_function_signature = typename _make_function_signature<RetType, ParamPackType, QualType>::_type;

		// Bind, generates a type wrapper for Template; evaluating this wrapper is equivalent to evaluating Template instantiated with some of its parameters bound to Types

	template <template <class ...> class Template, class Pack1, class Pack2>
	struct _bind : _bind<Template, typename Pack1::template pop<1>::template push<typename Pack1::first>, Pack2>
	{
	};

	template <template <class ...> class Template, std::size_t Index, class ... Types1, class Pack2>
	struct _bind<Template, pack<_implementation::_placeholder<Index>, Types1 ...>, Pack2> : _bind<Template, pack<Types1 ..., typename Pack2::template pop<Index - 1>::first>, Pack2>
	{
		static_assert(Index <= Pack2::size, 
					  "'stdx::meta::bind<Template, BoundTypes ...>::invoke<Types ...>': "
					  "For BoundTypes of type stdx::meta::_implementation::_placeholder<Index>, Index must be less or equal than sizeof...(Types)");
	};

	template <template <class ...> class Template, class ... Types1, class Pack2>
	struct _bind<Template, pack<_implementation::_undefined, Types1 ...>, Pack2>
	{
		using _invoke = Template<Types1 ...>;
	};

	template <template <class ...> class Template, class ... BoundTypes>
	struct bind
	{
		template <class ... Types>
		using invoke = typename _bind<Template, pack<BoundTypes ..., _implementation::_undefined>, pack<Types ...>>::_invoke;
	};

	// Atomic traits

		// Is lock free, determines if built-in atomic type is lock-free, assuming that it is properly aligned (needs reviewing)

	template <class>
	struct is_lock_free;

	template <class Type>
	struct is_lock_free<std::atomic<Type>> : std::false_type
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

	template <class Type>
	constexpr bool is_lock_free_v = is_lock_free<Type>::value;
}

#endif
