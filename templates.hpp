#ifndef STDX_TEMPLATES_HPP
#define STDX_TEMPLATES_HPP

#include <atomic>

namespace stdx::templates
{
	template <class, class>
	struct typepair;

	template <auto, auto>
	struct valuepair;

	template <class, auto>
	struct typevalue;

	template <class ...>
	struct pack;

	template <auto ...>
	struct valuetuple;

	// Type traits

		// Obtain template from class

	template <class>
	struct class_template;

	template <template <class ...> class Template, class ... Parameters>
	struct class_template<Template<Parameters ...>>
	{
		template <class ... Parameters1>
		using type = Template<Parameters1 ...>;
	};

	template <template <auto ...> class Template, auto ... Parameters>
	struct class_template<Template<Parameters ...>>
	{
		template <auto ... Parameters1>
		using type = Template<Parameters1 ...>;
	};

		// Obtain parameter from class

	template <class>
	struct class_parameter;

	template <template <class ...> class Template, class ... Parameters>
	struct class_parameter<Template<Parameters ...>>
	{
		template <size_t N>
		using type = typename pack<Parameters ...>::template pop<N>::first;
	};

	template <template <auto ...> class Template, auto ... Parameters>
	struct class_parameter<Template<Parameters ...>>
	{
		template <size_t N>
		static constexpr auto value = valuetuple<Parameters ...>::template pop<N>::first;
	};

	template <template <class> class Template, class Parameter>
	struct class_parameter<Template<Parameter>>
	{
		using type = Parameter;
	};

	template <template <auto> class Template, auto Parameter>
	struct class_parameter<Template<Parameter>>
	{
		static constexpr auto value = Parameter;
	};

/*	Note: (Potential bug)
	
	transformed_typevaluetuple <class_parameter, pack<stp::_placeholder<1>>> foo; 
	_transformed_typevaluetuple <class_parameter, pack<stp::_placeholder<1>>, valuetuple<>> foo;

	Does not compile unless specialization class_parameter<Template<Parameter>> is defined as such:

	template <template <int> class Template, int Parameter>
	struct class_parameter<Template<Parameter>>
	{
		static constexpr auto value = Parameter;
	};

*/

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

		// Function signature

	template <class>
	struct function_signature;

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...)>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) &>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) &&>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &&>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &&>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &&>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) & noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const & noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile & noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile & noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) && noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const && noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile && noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile && noexcept>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class FuncType>
	struct function_signature<FuncType *> : function_signature<FuncType>
	{
	};

	template <class FuncType, class ObjType>
	struct function_signature<FuncType ObjType::*> : function_signature<FuncType>
	{
		using object_type = ObjType;
	};

	// Numeric traits

		// Addition

	template <auto X, auto Y>
	struct _addition
	{
		static constexpr auto value = X + Y;
	};

	template <auto Y>
	struct addition
	{
		template <auto X>
		using operation = _addition<X, Y>;
	};

		// Subtraction

	template <auto X, auto Y>
	struct _subtraction
	{
		static constexpr auto value = X - Y;
	};

	template <auto Y>
	struct subtraction
	{
		template <auto X>
		using operation = _subtraction<X, Y>;
	};

		// Multiplication

	template <auto X, auto Y>
	struct _multiplication
	{
		static constexpr auto value = X * Y;
	};

	template <auto Y>
	struct multiplication
	{
		template <auto X>
		using operation = _multiplication<X, Y>;
	};

		// Division

	template <auto X, auto Y>
	struct _division
	{
		static constexpr auto value = X / Y;
	};

	template <auto Y>
	struct division
	{
		template <auto X>
		using operation = _division<X, Y>;
	};

/*	Note: (Potential bug)

	template <auto Value>
	struct _operation
	{
		static constexpr auto value = Value;
	};

	template <auto Y>
	struct multiplication
	{
		template <auto X>
		using operation = _operation<X * Y>;
	};

	// Division

	template <auto Y>
	struct division
	{
		template <auto X>
		using operation = _operation<X / Y>;
	};
*/
		// Range

	template <auto N, auto Min, auto Max>
	struct _between : std::bool_constant<bool(Min <= N && N <= Max)>
	{
	};

	template <auto N, auto Min, auto Max>
	struct _beside : std::bool_constant<bool(N < Min || Max < N)>
	{
	};
	
	template <auto Min, auto Max>
	struct range
	{
		template <auto N>
		using between = _between<N, Min, Max>;
		template <auto N>
		using beside = _beside<N, Min, Max>;
	};

/*	Note: (Potential bug)
	
	Cast any one of these to an integral type (or otherwise) and the compiler breaks

*/

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

/*	template <class Type1, class Type2>
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
	}; */

	template <auto N>
	struct value
	{
		static constexpr auto value = N;
	};

		// OutPack

	template <class ...>
	struct pack
	{
		static constexpr size_t size = 0;
		template <class ... Types>
		using push = pack<Types ...>;
		template <size_t N>
		using pop = pack<>;
	};

	template <class Type>
	struct pack<Type>
	{
		using first = Type;
		using last = Type;
		static constexpr size_t size = 1;
		template <class ... Types>
		using push = pack<Type, Types ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), pack<>, pack<Type>>;
	};

	template <class Type, class ... Types>
	struct pack<Type, Types ...>
	{
		using first = Type;
		using last = typename pack<Types ...>::last;
		static constexpr size_t size = 1 + sizeof...(Types);
		template <class ... InPackFirstRest>
		using push = pack<Type, Types ..., InPackFirstRest ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename pack<Types ...>::template pop<N - 1>, pack<Type, Types ...>>;
	};

/*		// IndexPack

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
	}; */

		// Constrained pack, applies a trait to each element of a pack (which can be of type typepair), constructing a new pack with the elements of the old one (or the second types of the typepairs) for which bool(trait<element>::value) == true (or bool(trait<element::first>::value) == true)

	template <template <class> class Trait, class InPack, class OutPack>
	struct _constrained_pack : _constrained_pack<Trait, typename InPack::template pop<1>, std::conditional_t<bool(Trait<typename InPack::first>::value), typename OutPack::template push<typename InPack::first>, OutPack>>
	{
	};

	template <template <class> class Trait, class InPackFirst, class ... InPackFirstRest, class ... InPackRest, class OutPack> // Note: this feels inconsistent (Idea: create a unique constrained_typepairtuple)
	struct _constrained_pack<Trait, pack<pack<InPackFirst, InPackFirstRest ...>, InPackRest ...>, OutPack> : _constrained_pack<Trait, pack<InPackRest ...>, std::conditional_t<bool(Trait<InPackFirst>::value), typename OutPack::template push<pack<InPackFirst, InPackFirstRest ...>>, OutPack>>
	{
	};

	template <template <class> class Trait, class OutPack>
	struct _constrained_pack<Trait, pack<>, OutPack>
	{
		using type = OutPack;
	};

	template <template <class> class Trait, class InPack>
	struct _assert_constrained_pack : _constrained_pack<Trait, InPack, pack<>>
	{
		static_assert(is_same_as_v<InPack, pack>,
					  "stdx::templates::constrained_pack<Trait, InPack>: InPack must be of type stdx::templates::pack<T ...> where T is any type parameter");
	};

	template <template <class> class Trait, class InPack>
	using constrained_pack = typename _assert_constrained_pack<Trait, InPack>::type;
/*
		// Constrained valuetuple, applies a trait to each element of a valuetuple, constructing a new valuetuple with the elements of the old one for which bool(trait<element>::value) == true

	template <template <auto> class Trait, class Valuetuple1, class Valuetuple2>
	struct _constrained_valuetuple : _constrained_valuetuple<Trait, typename Valuetuple1::template pop<1>, std::conditional_t<bool(Trait<Valuetuple1::first>::value), typename Valuetuple2::template push<Valuetuple1::first>, Valuetuple2>>
	{
	};

	template <template <auto> class Trait, class IndexPack>
	struct _constrained_valuetuple<Trait, valuetuple<>, IndexPack>
	{
		using type = IndexPack;
	};

	template <template <auto> class Trait, class IndexPack>
	struct _assert_constrained_valuetuple : _constrained_valuetuple<Trait, IndexPack, valuetuple<>>
	{
		static_assert(_is_same_as_v<IndexPack, valuetuple>,
					  "stdx::templates::constrained_valuetuple<Trait, IndexPack>: OutPack must be of type stdx::templates::valuetuple<V ...> where V is any non-type parameter");
	};

	template <template <auto> class Trait, class IndexPack>
	using constrained_valuetuple = typename _assert_constrained_valuetuple<Trait, IndexPack>::type;

		// Constrained typevaluetuple, applies a trait to each element of a pack (which must be of type typevalue), constructing a valuetuple with the elements for which bool(trait<element::first>::value) == true

	template <template <class> class Trait, class OutPack, class IndexPack> // Note: this feels inconsistent
	struct _constrained_typevaluetuple : _constrained_typevaluetuple<Trait, typename OutPack::template pop<1>, std::conditional_t<bool(Trait<typename OutPack::first::first>::value), typename IndexPack::template push<OutPack::first::second>, IndexPack>>
	{
	};

	template <template <class> class Trait, class IndexPack>
	struct _constrained_typevaluetuple<Trait, pack<>, IndexPack>
	{
		using type = IndexPack;
	};

	template <template <class> class Trait, class OutPack>
	struct _assert_constrained_typevaluetuple : _constrained_typevaluetuple<Trait, OutPack, valuetuple<>>
	{
		static_assert(is_same_as_v<OutPack, pack>,
					  "stdx::templates::constrained_typevaluetuple<Trait, OutPack>: OutPack must be of type stdx::templates::pack<TV ...> where TV is of type stdx::templates::typevalue<T, V> and where T is any type paramter and V is any non-type parameter");
	};

	template <template <class> class Trait, class OutPack>
	using constrained_typevaluetuple = typename _assert_constrained_typevaluetuple<Trait, OutPack>::type;
*/
		// Transformed pack, applies a trait to each element of a pack, constructing a new pack of the same size as the old one and with the corresponding elements as trait<element>::type
	
	template <template <class> class Trait, class InPack, class OutPack>
	struct _transformed_pack : _transformed_pack<Trait, typename InPack::template pop<1>, typename OutPack::template push<typename Trait<typename InPack::first>::type>>
	{
	};

	template <template <class> class Trait, class OutPack>
	struct _transformed_pack<Trait, pack<>, OutPack>
	{
		using type = OutPack;
	};

	template <template <class> class Trait, class InPack>
	struct _assert_transformed_pack : _transformed_pack<Trait, InPack, pack<>>
	{
		static_assert(is_same_as_v<InPack, pack>,
					  "stdx::templates::transformed_pack<Trait, InPack>: InPack must be of type stdx::templates::pack<T ...> where T is any type parameter");
	};

	template <template <class> class Trait, class InPack>
	using transformed_pack = typename _assert_transformed_pack<Trait, InPack>::type;
/*
		// Transformed valuetuple, applies a trait to each element of a valuetuple, constructing a new valuetuple of the same size as the old one and with the corresponding elements as trait<element>::value

	template <template <auto> class Trait, class Valuetuple1, class Valuetuple2>
	struct _transformed_valuetuple : _transformed_valuetuple<Trait, typename Valuetuple1::template pop<1>, typename Valuetuple2::template push<Trait<Valuetuple1::first>::value>>
	{
	};

	template <template <auto> class Trait, class IndexPack>
	struct _transformed_valuetuple<Trait, valuetuple<>, IndexPack>
	{
		using type = IndexPack;
	};

	template <template <auto> class Trait, class IndexPack>
	struct _assert_transformed_valuetuple : _transformed_valuetuple<Trait, IndexPack, valuetuple<>>
	{
		static_assert(_is_same_as_v<IndexPack, valuetuple>,
					  "stdx::templates::transformed_valuetuple<Trait, IndexPack>: IndexPack must be of type stdx::templates::valuetuple<V ...> where V is any non-type parameter");
	};

	template <template <auto> class Trait, class IndexPack>
	using transformed_valuetuple = typename _assert_transformed_valuetuple<Trait, IndexPack>::type;
	
		// Transformed typevaluetuple, applies a trait to each element of a pack, constructing a new valuetuple of the same size as the old one and with the corresponding elements as trait<element>::value

	template <template <class> class Trait, class OutPack, class IndexPack>
	struct _transformed_typevaluetuple : _transformed_typevaluetuple<Trait, typename OutPack::template pop<1>, typename IndexPack::template push<Trait<typename OutPack::first>::value>>
	{
	};

	template <template <class> class Trait, class IndexPack>
	struct _transformed_typevaluetuple<Trait, pack<>, IndexPack>
	{
		using type = IndexPack;
	};

	template <template <class> class Trait, class OutPack>
	struct _assert_transformed_typevaluetuple : _transformed_typevaluetuple<Trait, OutPack, valuetuple<>>
	{
		static_assert(is_same_as_v<OutPack, pack>,
					  "stdx::templates::transformed_typevaluetuple<Trait, OutPack>: OutPack must be of type stdx::templates::pack<T ...> where T is any type parameter");
	};

	template <template <class> class Trait, class OutPack>
	using transformed_typevaluetuple = typename _assert_transformed_typevaluetuple<Trait, OutPack>::type;
*/
		// Permutated tuple, permutates the parameters inside a pack based on a valuetuple with integral values in the interval [0, N) where N is the number of elements in the tuples

	template <class InPack, class OutPack, class IndexPack>
	struct _permutated_pack : _permutated_pack<InPack, typename OutPack::template push<typename InPack::template pop<IndexPack::first::value>::first>, typename IndexPack::template pop<1>>
	{
	};

	template <class InPack, class OutPack>
	struct _permutated_pack<InPack, OutPack, pack<>>
	{
		using type = OutPack;
	};

	template <class>
	struct _assert_index_pack_permutated_pack : std::false_type
	{
	};

	template <auto ... Values>
	struct _assert_index_pack_permutated_pack<pack<value<Values> ...>> : std::is_same<pack<value<Values> ...>, constrained_pack<range<0, sizeof...(Values) - 1>::template between, constrained_pack<std::is_integral, pack<value<Values> ...>>>> // Review pending
	{
	};

	template <class InPack, class IndexPack>
	struct _assert_permutated_pack : _permutated_pack<InPack, pack<>, IndexPack>
	{
		static_assert(is_same_as_v<InPack, pack> || _is_same_as_v<InPack, valuetuple>,
					  "stdx::templates::reordered_typetuple<InPack, IndexPack>: InPack must be of type stdx::templates::pack<T ...>");
		static_assert(InPack::size == IndexPack::size,
					  "stdx::templates::reordered_typetuple<InPack, IndexPack>: InPack::size must be equal to IndexPack::size");
		static_assert(_assert_index_pack_permutated_pack<IndexPack>::value,
					  "stdx::templates::reordered_typetuple<InPack, IndexPack>: IndexPack must be of type stdx::template::pack<stdx::template::value<V> ...> type where V is an integral type in the interval of [0, IndexPack::size)");
	};

	template <class InPack, class IndexPack>
	using permutated_pack = typename _assert_permutated_pack<InPack, IndexPack>::type;
}

#endif
