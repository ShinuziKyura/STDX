#ifndef STDX_META_HPP
#define STDX_META_HPP

#include <atomic>

namespace stdx::meta
{
	template <auto>
	struct val;

	template <class ...>
	struct pack;

	template <auto ...>
	struct valpack;

	// Type traits

		// Identity trait, provides type equal to the type which it was instantiated

	template <class Type>
	struct identity
	{
		using type = Type;
	};

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

		// Apply a trait that takes a type parameter to type aliases from Val and Pack

	template <template <class> class Trait>
	struct type_trait
	{
		template <class Val>
		using type = Trait<typename Val::type>;
		template <class Pack>
		using first = Trait<typename Pack::first>;
		template <class Pack>
		using last = Trait<typename Pack::last>;
	};

		// Apply a trait that takes a non-type parameter to static constexpr variables from Val and Valpack

	template <template <auto> class Trait>
	struct value_trait
	{
		template <class Val>
		using value = Trait<Val::value>;
		template <class Valpack>
		using first = Trait<Valpack::first>;
		template <class Valpack>
		using last = Trait<Valpack::last>;
	};

		// Cast from one type to another, if they are convertible

	template <class Type1, class Type2>
	using type_cast = std::enable_if_t<std::is_convertible_v<Type1, Type2>, Type2>;

		// Class-template equality

	template <class, template <class ...> class>
	struct is_same_as : std::false_type
	{
	};

	template <template <class ...> class Template, class ... Parameters>
	struct is_same_as<Template<Parameters ...>, Template> : std::true_type
	{
	};

	template <class Class, template <class ...> class Template>
	inline constexpr bool is_same_as_v = is_same_as<Class, Template>::value;

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

		// Tag types for function_signature and make_function_signature

	struct signature_;
	struct signature_c;
	struct signature_v;
	struct signature_cv;
	struct signature_l;
	struct signature_cl;
	struct signature_vl;
	struct signature_cvl;
	struct signature_r;
	struct signature_cr;
	struct signature_vr;
	struct signature_cvr;
	struct signature_n;
	struct signature_cn;
	struct signature_vn;
	struct signature_cvn;
	struct signature_ln;
	struct signature_cln;
	struct signature_vln;
	struct signature_cvln;
	struct signature_rn;
	struct signature_crn;
	struct signature_vrn;
	struct signature_cvrn;

		// Function signature

	template <class>
	struct _function_signature;

	template <class RetType, class ... ParamTypes>
	struct _function_signature<RetType(ParamTypes ...)>
	{
		using return_type = RetType;
		using parameter_types = pack<ParamTypes ...>;
	};

	template <class>
	struct function_signature;

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...)> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_c, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_v, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cv, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) &> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_l, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cl, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_vl, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cvl, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) && > : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_r, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const &&> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cr, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile &&> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_vr, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile &&> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cvr, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_n, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_vn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cvn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) & noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_ln, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const & noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cln, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile & noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_vln, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile & noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cvln, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) && noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_rn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const && noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_crn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) volatile && noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_vrn, pack<ParamTypes ...>>;
	};

	template <class RetType, class ... ParamTypes>
	struct function_signature<RetType(ParamTypes ...) const volatile && noexcept> : _function_signature<RetType(ParamTypes ...)>
	{
		using type = pack<RetType, signature_cvrn, pack<ParamTypes ...>>;
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
	struct _make_function_signature<pack<RetType, signature_, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...);
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_c, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_v, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cv, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_l, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cl, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_vl, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cvl, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile &;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_r, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cr, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_vr, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cvr, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile &&;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_n, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_vn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cvn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_ln, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cln, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_vln, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cvln, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile & noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_rn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_crn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_vrn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) volatile && noexcept;
	};

	template <class RetType, class ... ParamTypes>
	struct _make_function_signature<pack<RetType, signature_cvrn, pack<ParamTypes ...>>>
	{
		using type = RetType(ParamTypes ...) const volatile && noexcept;
	};

	template <class FuncType>
	using make_function_signature = typename _make_function_signature<FuncType>::type;

	// Numeric traits

		// Addition

	template <auto Y>
	struct addition
	{
		template <auto X>
		using trait = std::integral_constant<decltype(X), X + Y>;
	};

		// Subtraction

	template <auto Y>
	struct subtraction
	{
		template <auto X>
		using trait = std::integral_constant<decltype(X), X - Y>;
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
	struct between
	{
		using _auto = decltype(Min); // Workaround for VC++ bug
		template <_auto N>
		using trait = std::bool_constant<Min <= N && N <= Max>;
	};

	template <auto Min, auto Max>
	struct beside
	{
		using _auto = decltype(Min); // Workaround for VC++ bug
		template <_auto N>
		using trait = std::bool_constant<N < Min || Max < N>;
	};

	// Logic traits

		// Conjunction

	template <class ... BoolTypes>
	inline constexpr bool _conjunction(BoolTypes ... booleans)
	{
		return (... && booleans);
	}

	template <template <class> class Trait>
	struct conjunction
	{
		template <class ... Types>
		using trait = std::bool_constant<_conjunction(Trait<Types>::value ...)>;
	};

		// Disjunction

	template <class ... BoolTypes>
	inline constexpr bool _disjunction(BoolTypes ... booleans)
	{
		return (... || booleans);
	}

	template <template <class> class Trait>
	struct disjunction
	{
		template <class ... Types>
		using trait = std::bool_constant<_disjunction(Trait<Types>::value ...)>;
	};

/*	
	Note: (Potential bug)
	
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

		// Vals

	template <auto N>
	struct val
	{
		using type = decltype(N);
		static constexpr auto value = N;
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
		static constexpr auto first = Value;
		static constexpr auto last = Value;
		static constexpr size_t size = 1;
		template <auto ... Values>
		using push = valpack<Value, Values ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), valpack<>, valpack<Value>>;
	};

	template <auto Value, auto ... Values>
	struct valpack<Value, Values ...>
	{
		static constexpr auto first = Value;
		static constexpr auto last = valpack<Values ...>::last;
		static constexpr size_t size = 1 + sizeof...(Values);
		template <auto ... Values1>
		using push = valpack<Value, Values ..., Values1 ...>;
		template <size_t N>
		using pop = std::conditional_t<bool(N), typename valpack<Values ...>::template pop<N - 1>, valpack<Value, Values ...>>;
	};

		// Create Val from static constexpr value

	template <class Type>
	struct as_val
	{
		using type = val<Type::value>;
	};

		// Convert from Pack of Vals to Valpack

	template <class>
	struct _as_valpack;

	template <auto ... Values>
	struct _as_valpack<valpack<Values ...>>
	{
		using type = valpack<Values ...>;
	};

	template <auto ... Values>
	struct _as_valpack<pack<val<Values> ...>>
	{
		using type = valpack<Values ...>;
	};

	template <>
	struct _as_valpack<pack<>>
	{
		using type = valpack<>;
	};

	template <class Pack>
	using as_valpack = typename _as_valpack<Pack>::type;

		// Convert from Valpack to Pack of Vals

	template <class>
	struct _as_pack_val;

	template <auto ... Values>
	struct _as_pack_val<pack<val<Values> ...>>
	{
		using type = pack<val<Values> ...>;
	};

	template <>
	struct _as_pack_val<pack<>>
	{
		using type = pack<>;
	};

	template <auto ... Values>
	struct _as_pack_val<valpack<Values ...>>
	{
		using type = pack<val<Values> ...>;
	};

	template <class Pack>
	using as_pack_val = typename _as_pack_val<Pack>::type;

		// Constrained pack, applies a trait to each element of a pack, constructing a new pack with the elements of the old one for which bool(trait<element>::value) == true

	template <template <class> class Trait, class InPack, class OutPack>
	struct _constrained_pack : _constrained_pack<Trait, typename InPack::template pop<1>, std::conditional_t<bool(Trait<typename InPack::first>::value), typename OutPack::template push<typename InPack::first>, OutPack>>
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
					  "'stdx::meta::constrained_pack<Trait, InPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T is any type parameter");
	};

	template <template <class> class Trait, class InPack>
	using constrained_pack = typename _assert_constrained_pack<Trait, InPack>::type;

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
					  "'stdx::meta::transformed_pack<Trait, InPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T is any type parameter");
	};

	template <template <class> class Trait, class InPack>
	using transformed_pack = typename _assert_transformed_pack<Trait, InPack>::type;

		// Permutated pack, permutates the elements of a pack based on a valpack with integral values in the interval [0, N) where N is the number of elements in the pack

	template <class InPack, class OutPack, class IndexPack>
	struct _permutated_pack : _permutated_pack<InPack, typename OutPack::template push<typename InPack::template pop<IndexPack::first>::first>, typename IndexPack::template pop<1>>
	{
	};

	template <class InPack, class OutPack>
	struct _permutated_pack<InPack, OutPack, valpack<>>
	{
		using type = OutPack;
	};

	template <class IndexPack>
	struct _assert_index_values_permutated_pack : std::is_same<IndexPack, constrained_pack<value_trait<between<0, IndexPack::size - 1>::trait>::template value, constrained_pack<type_trait<std::is_integral>::template type, IndexPack>>>
	{
	};

	template <class InPack, class IndexPack>
	struct _assert_permutated_pack : _permutated_pack<InPack, pack<>, as_valpack<IndexPack>>
	{
		static_assert(is_same_as_v<InPack, pack>,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "InPack must be of type stdx::meta::pack<T ...> where T is any type parameter");
		static_assert(InPack::size == IndexPack::size,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "InPack::size must be equal to IndexPack::size");
		static_assert(_assert_index_values_permutated_pack<as_pack_val<IndexPack>>::value,
					  "'stdx::meta::permutated_pack<InPack, IndexPack>': "
					  "IndexPack must be of type stdx::template::valpack<V ...> type where V are integral types in the interval of [0, IndexPack::size), all unique");
	};

	template <class InPack, class IndexPack>
	using permutated_pack = typename _assert_permutated_pack<InPack, IndexPack>::type;

		// Merged pack, merges several packs of the same size into one pack of packs where the nth pack contains the nth elements of each original pack

	template <size_t N, class OutPack, class ... InPack>
	struct _merged_pack : _merged_pack<N - 1, typename OutPack::template push<pack<typename InPack::first ...>>, typename InPack::template pop<1> ...>
	{
	};

	template <class OutPack, class ... InPack>
	struct _merged_pack<0, OutPack, InPack ...>
	{
		using type = OutPack;
	};

	template <class SizeType, class ... SizeTypes>
	inline constexpr bool _assert_pack_sizes_merged_pack(SizeType size, SizeTypes ... sizes)
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
	using merged_pack = typename _assert_merged_pack<InPack ...>::type;
}

#endif
