#ifndef STDX_META_HPP
#define STDX_META_HPP

#include <type_traits>
#include <complex>
#include <atomic>

namespace stdx::meta // TODO Consider further subdivision of file into nested namespaces (or into different files) for each trait class
{
	struct _nulltype;

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
		static constexpr size_t size = 0;

		template <class ... Types>
		using push = pack<Types ...>;
		template <size_t N>
		using pop = pack<>;
	};

	template <class Type>
	struct pack<Type>
	{
		static constexpr size_t size = 1;
		using first = Type;
		using last = Type;

		template <class ... Types>
		using push = pack<Type, Types ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), pack<>, pack<Type>>;
	};

	template <class Type, class ... Types>
	struct pack<Type, Types ...>
	{
		static constexpr size_t size = 1 + sizeof...(Types);
		using first = Type;
		using last = typename pack<Types ...>::last;

		template <class ... Types1>
		using push = pack<Type, Types ..., Types1 ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename pack<Types ...>::template pop<N - 1>, pack<Type, Types ...>>;
	};

		// Valpacks

	template <auto ...>
	struct valpack
	{
		static constexpr size_t size = 0;

		template <auto ... Values>
		using push = valpack<Values ...>;
		template <size_t N>
		using pop = pack<>;
	};

	template <auto Value>
	struct valpack<Value>
	{
		static constexpr size_t size = 1;
		static constexpr auto first = Value;
		static constexpr auto last = Value;

		template <auto ... Values>
		using push = valpack<Value, Values ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), valpack<>, valpack<Value>>;
	};

	template <auto Value, auto ... Values>
	struct valpack<Value, Values ...>
	{
		static constexpr size_t size = 1 + sizeof...(Values);
		static constexpr auto first = Value;
		static constexpr auto last = valpack<Values ...>::last;

		template <auto ... Values1>
		using push = valpack<Value, Values ..., Values1 ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename valpack<Values ...>::template pop<N - 1>, valpack<Value, Values ...>>;
	};

	// Type traits

		// Identity trait, provides type alias for the type with which it was instantiated

	template <class Type>
	struct identity
	{
		using type = Type;
	};
/*	TODO: review names
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
		using type = val<valpack<Parameters ...>::template pop<N>::first>;
	};
*/
		// Cast from one type to another, if they are convertible

	template <class Type1, class Type2>
	using type_cast = std::enable_if_t<std::is_convertible_v<Type1, Type2>, Type2>;

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
	inline constexpr bool is_template_instantiation_v = is_template_instantiation<Class, Template>::value;

	// Function traits

		// Tag types for function_signature and make_function_signature

	struct qualifiers_;
	struct qualifiers_c;
	struct qualifiers_v;
	struct qualifiers_cv;
	struct qualifiers_l;
	struct qualifiers_cl;
	struct qualifiers_vl;
	struct qualifiers_cvl;
	struct qualifiers_r;
	struct qualifiers_cr;
	struct qualifiers_vr;
	struct qualifiers_cvr;
	struct qualifiers_n;
	struct qualifiers_cn;
	struct qualifiers_vn;
	struct qualifiers_cvn;
	struct qualifiers_ln;
	struct qualifiers_cln;
	struct qualifiers_vln;
	struct qualifiers_cvln;
	struct qualifiers_rn;
	struct qualifiers_crn;
	struct qualifiers_vrn;
	struct qualifiers_cvrn;

		// Function signature

	template <class, class>
	struct _function_signature;

	template <class RetType, class ... ParamTypes, class QualType>
	struct _function_signature<RetType(ParamTypes ...), QualType>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
		using qualifiers_type = QualType;
	};

	template <class>
	struct function_signature;

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...)> : _function_signature<RetType(ParamTypes ...), qualifiers_>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const> : _function_signature<RetType(ParamTypes ...), qualifiers_c>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile> : _function_signature<RetType(ParamTypes ...), qualifiers_v>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile> : _function_signature<RetType(ParamTypes ...), qualifiers_cv>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) &> : _function_signature<RetType(ParamTypes ...), qualifiers_l>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &> : _function_signature<RetType(ParamTypes ...), qualifiers_cl>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &> : _function_signature<RetType(ParamTypes ...), qualifiers_vl>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &> : _function_signature<RetType(ParamTypes ...), qualifiers_cvl>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) &&> : _function_signature<RetType(ParamTypes ...), qualifiers_r>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &&> : _function_signature<RetType(ParamTypes ...), qualifiers_cr>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &&> : _function_signature<RetType(ParamTypes ...), qualifiers_vr>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &&> : _function_signature<RetType(ParamTypes ...), qualifiers_cvr>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_n>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_cn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_vn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_cvn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) & noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_ln>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const & noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_cln>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile & noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_vln>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile & noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_cvln>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) && noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_rn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const && noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_crn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile && noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_vrn>
	{
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile && noexcept> : _function_signature<RetType(ParamTypes ...), qualifiers_cvrn>
	{
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

		// Make function signature

	template <class>
	struct _make_function_signature;

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_>>
	{
		using _type = RetType(ParamTypes ...);
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_c>>
	{
		using _type = RetType(ParamTypes ...) const;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_v>>
	{
		using _type = RetType(ParamTypes ...) volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cv>>
	{
		using _type = RetType(ParamTypes ...) const volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_l>>
	{
		using _type = RetType(ParamTypes ...) &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cl>>
	{
		using _type = RetType(ParamTypes ...) const &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_vl>>
	{
		using _type = RetType(ParamTypes ...) volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cvl>>
	{
		using _type = RetType(ParamTypes ...) const volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_r>>
	{
		using _type = RetType(ParamTypes ...) &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cr>>
	{
		using _type = RetType(ParamTypes ...) const &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_vr>>
	{
		using _type = RetType(ParamTypes ...) volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cvr>>
	{
		using _type = RetType(ParamTypes ...) const volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_n>>
	{
		using _type = RetType(ParamTypes ...) noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cn>>
	{
		using _type = RetType(ParamTypes ...) const noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_vn>>
	{
		using _type = RetType(ParamTypes ...) volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cvn>>
	{
		using _type = RetType(ParamTypes ...) const volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_ln>>
	{
		using _type = RetType(ParamTypes ...) & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cln>>
	{
		using _type = RetType(ParamTypes ...) const & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_vln>>
	{
		using _type = RetType(ParamTypes ...) volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cvln>>
	{
		using _type = RetType(ParamTypes ...) const volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_rn>>
	{
		using _type = RetType(ParamTypes ...) && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_crn>>
	{
		using _type = RetType(ParamTypes ...) const && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_vrn>>
	{
		using _type = RetType(ParamTypes ...) volatile && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, pack<ParamTypes ...>, qualifiers_cvrn>>
	{
		using _type = RetType(ParamTypes ...) const volatile && noexcept;
	};

	template <class FuncType>
	using make_function_signature = typename _make_function_signature<FuncType>::_type;

	// Numeric traits

		// Boolean checks

	template <class Type>
	struct is_complex : std::false_type
	{
	};

	template <class Type>
	struct is_complex<std::complex<Type>> : std::true_type
	{
	};

	template <class Type>
	inline constexpr bool is_complex_v = is_complex<Type>::value;

		// Addition

	template <auto Y>
	struct addition
	{
		template <auto X>
		using trait = std::integral_constant<decltype(X + Y), X + Y>;
	};

		// Subtraction

	template <auto Y>
	struct subtraction
	{
		template <auto X>
		using trait = std::integral_constant<decltype(X - Y), X - Y>;
	};

		// Multiplication

	template <auto Y>
	struct multiplication
	{
		using _auto = decltype(Y); // Workaround for VC++ bug
		template <_auto X>
		using trait = std::integral_constant<_auto, X * Y>;
	};

		// Division

	template <auto Y>
	struct division
	{
		using _auto = decltype(Y); // Workaround for VC++ bug
		template <_auto X>
		using trait = std::integral_constant<_auto, X / Y>;
	};

		// Range

	template <auto Min, auto Max>
	struct inside
	{
		using _auto = decltype(Min); // Workaround for VC++ bug
		template <_auto N>
		using trait = std::bool_constant<Min <= N && N <= Max>;
	};

	template <auto Min, auto Max>
	struct outside
	{
		using _auto = decltype(Min); // Workaround for VC++ bug
		template <_auto N>
		using trait = std::bool_constant<N < Min || Max < N>;
	};

	// Logic traits
	
		// If statement, takes the first type for which corresponding condition is true, or _nulltype (an incomplete type) if all are false

	template <bool, class>
	struct _type_if;

	template <class Type>
	struct _type_then
	{
		template <bool>
		using else_if = _type_if<false, Type>;

		template <class>
		using else_then = _type_then<Type>;

		using endif = Type;
	};

	template <>
	struct _type_then<_nulltype>
	{
		template <bool Condition>
		using else_if = _type_if<Condition, _nulltype>;

		template <class Type>
		using else_then = _type_then<Type>;

		using endif = _nulltype;
	};

	template <bool, class Type>
	struct _type_if
	{
		template <class>
		using then = _type_then<Type>;
	};

	template <>
	struct _type_if<false, _nulltype>
	{
		template <class Type>
		using then = _type_then<_nulltype>;
	};

	template <>
	struct _type_if<true, _nulltype>
	{
		template <class Type>
		using then = _type_then<Type>;
	};

	template <bool Condition>
	using type_if = _type_if<Condition, _nulltype>;

		// Conjunction // Review this (make interface more similar to std versions)

	template <class ... BoolTypes>
	constexpr bool _conjunction(BoolTypes ... booleans)
	{
		return (... && booleans);
	}

	template <template <class> class Trait>
	struct conjunction
	{
		template <class ... Types>
		using trait = std::bool_constant<_conjunction(Trait<Types>::value ...)>;
	};

		// Disjunction // Review this (make interface more similar to std versions)

	template <class ... BoolTypes>
	constexpr bool _disjunction(BoolTypes ... booleans)
	{
		return (... || booleans);
	}

	template <template <class> class Trait>
	struct disjunction
	{
		template <class ... Types>
		using trait = std::bool_constant<_disjunction(Trait<Types>::value ...)>;
	};

	// Atomic traits

		// Determines if built-in atomic type is lock-free, assuming that it is properly aligned

	template <class>
	struct is_lock_free;

	template <class Type>
	struct is_lock_free<std::atomic<Type>> : std::false_type // Review this (hint: determine by appropriate is_lock_free<std::atomic<integral> based on sizeof(Type))
	{
	};

#ifdef ATOMIC_BOOL_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<bool>> : std::integral_constant<int, ATOMIC_BOOL_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_CHAR_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char>> : std::integral_constant<int, ATOMIC_CHAR_LOCK_FREE>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned char>> : std::integral_constant<int, ATOMIC_CHAR_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_CHAR16_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char16_t>> : std::integral_constant<int, ATOMIC_CHAR16_T_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_CHAR32_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<char32_t>> : std::integral_constant<int, ATOMIC_CHAR32_T_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_WCHAR_T_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<wchar_t>> : std::integral_constant<int, ATOMIC_WCHAR_T_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_SHORT_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<short>> : std::integral_constant<int, ATOMIC_SHORT_LOCK_FREE>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned short>> : std::integral_constant<int, ATOMIC_SHORT_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_INT_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<int>> : std::integral_constant<int, ATOMIC_INT_LOCK_FREE>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned int>> : std::integral_constant<int, ATOMIC_INT_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_LONG_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<long>> : std::integral_constant<int, ATOMIC_LONG_LOCK_FREE>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned long>> : std::integral_constant<int, ATOMIC_LONG_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_LLONG_LOCK_FREE
	template <>
	struct is_lock_free<std::atomic<long long>> : std::integral_constant<int, ATOMIC_LLONG_LOCK_FREE>
	{
	};

	template <>
	struct is_lock_free<std::atomic<unsigned long long>> : std::integral_constant<int, ATOMIC_LLONG_LOCK_FREE>
	{
	};
#endif

#ifdef ATOMIC_POINTER_LOCK_FREE
	template <class Type>
	struct is_lock_free<std::atomic<Type *>> : std::integral_constant<int, ATOMIC_POINTER_LOCK_FREE>
	{
	};
#endif

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

	template <class Pack>
	using as_pack_val = typename _as_pack_val<Pack>::_type;

		// Apply a trait that takes a type parameter to the type from a Val

	template <template <class> class Trait>
	struct type_trait
	{
		template <class Val>
		using type = Trait<typename Val::type>;
	};

		// Apply a trait that takes a non-type parameter to the value from a Val

	template <template <auto> class Trait>
	struct value_trait
	{
		template <class Val>
		using value = Trait<Val::value>;
	};

		// Apply a trait that takes a type parameter to a type from a Pack

	template <template <class> class Trait, size_t Index = 0>
	struct pack_trait
	{
		template <class Pack>
		using first = Trait<typename Pack::first>;
		template <class Pack>
		using nth = Trait<typename Pack::template pop<Index>::first>;
		template <class Pack>
		using last = Trait<typename Pack::last>;
	};

		// Apply a trait that takes a non-type parameter to a value from a Valpack

	template <template <auto> class Trait, size_t Index = 0>
	struct valpack_trait
	{
		template <class Valpack>
		using first = Trait<Valpack::first>;
		template <class Valpack>
		using nth = Trait<Valpack::template pop<Index>::first>;
		template <class Valpack>
		using last = Trait<Valpack::last>;
	};

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
	struct _assert_index_values_permutated_pack : std::is_same<IndexPack, constrained_pack<value_trait<inside<0, IndexPack::size - 1>::template trait>::template value, constrained_pack<type_trait<std::is_integral>::template type, IndexPack>>>
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

	template <size_t N, class OutPack, class ... InPack>
	struct _merged_pack : _merged_pack<N - 1, typename OutPack::template push<pack<typename InPack::first ...>>, typename InPack::template pop<1> ...>
	{
	};

	template <class OutPack, class ... InPack>
	struct _merged_pack<0, OutPack, InPack ...>
	{
		using _type = OutPack;
	};

	template <class SizeType, class ... SizeTypes>
	constexpr bool _assert_pack_sizes_merged_pack(SizeType size, SizeTypes ... sizes)
	{
		return (... && (size == sizes));
	};

	template <class ... InPack>
	struct _assert_merged_pack : _merged_pack<pack<InPack ...>::first::size, pack<>, InPack ...>
	{
		static_assert(_assert_pack_sizes_merged_pack(InPack::size ...),
					  "'stdx::meta::merged_pack<InPack ...>': "
					  "InPack::size must be equal for all packs");
	};

	template <class ... InPack>
	using merged_pack = typename _assert_merged_pack<InPack ...>::_type;
}

#if defined(STDX_USING_META) || defined(STDX_USING_ALL)
namespace stdx { using namespace meta; }
#endif

#endif
