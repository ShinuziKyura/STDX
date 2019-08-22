#ifndef STDX_header_FUNCTIONAL
#define STDX_header_FUNCTIONAL

#include <functional>

#include "meta.hpp"

/* Informative error messages
template <class T>
struct AlwaysFalse
{
	static constexpr bool Value = false;
};

template <class T, class U = T>
struct ILikeIntsAndBools_WhyFail // Need at least one dependent name throughout the chain (aka no explicit / always partial specializations)
{
	static_assert(AlwaysFalse<U>::Value, "'You didn't give me an int or a bool'");
	static constexpr bool Value = false;
};

template <class U>
struct ILikeIntsAndBools_WhyFail<int, U>
{
	static_assert(AlwaysFalse<U>::Value, "'You only gave me an int'");
	static constexpr bool Value = false;
};

template <class U>
struct ILikeIntsAndBools_WhyFail<bool, U>
{
	static_assert(AlwaysFalse<U>::Value, "'You only gave me a bool'");
	static constexpr bool Value = false;
};

template <class T, class U = T>
struct ILikeIntsAndBools
{
	static constexpr bool HasInt = ILikeIntsAndBools_WhyFail<U>::Value;
	static constexpr bool HasBool = ILikeIntsAndBools_WhyFail<U>::Value;
};

template <class U>
struct ILikeIntsAndBools<int, U>
{
	static constexpr bool HasInt = true;
	static constexpr bool HasBool = ILikeIntsAndBools_WhyFail<int, U>::Value;
};

template <class U>
struct ILikeIntsAndBools<bool, U>
{
	static constexpr bool HasInt = ILikeIntsAndBools_WhyFail<bool, U>::Value;
	static constexpr bool HasBool = true;
};

int main()
{
	using IHaveInt = ILikeIntsAndBools<int>;
	static_assert(IHaveInt::HasInt, "'You should give it an int'");
//	static_assert(IHaveInt::HasBool, "'You should give it a bool'"); // Will show both assertions
	return 0;
}
*/

namespace stdx
{
#if !(defined(STDX_NAMESPACE_FUNCTIONAL) || defined(STDX_NAMESPACE_ALL))
inline
#endif
namespace functional
{
	template <class>
	class _lvalue_wrapper;

	template <class>
	class _xvalue_wrapper;

	namespace _implementation
	{
		namespace _value_category
		{
			struct _prvalue;
			struct _lvalue;
			struct _xvalue;
		}

			// _value

		template <class Type, class Category>
		struct _value
		{
			using type = Type;
			using category = Category;
		};

			// _value_category_wrapper

		template <class Type>
		class _value_category_wrapper // TODO possibly rename this
		{
		public:
			_value_category_wrapper(Type & object) noexcept
				: _object(std::addressof(object))
			{
			}
			_value_category_wrapper(_value_category_wrapper const &) = default;
			_value_category_wrapper(_value_category_wrapper &&) noexcept = default;
			_value_category_wrapper & operator=(_value_category_wrapper const &) = default;
			_value_category_wrapper & operator=(_value_category_wrapper &&) noexcept = default;

			operator Type &()
			{
				return *_object;
			}

		private:
			Type * _object;

		};

			// _make_value // TODO rethink this method, we may only need the type component

		template <class Type>
		struct _make_value : _value<Type, _value_category::_prvalue>
		{
		};

		template <class Type>
		struct _make_value<Type &> : _value<Type &, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _make_value<_lvalue_wrapper<Type>> : _value<Type, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _make_value<_lvalue_wrapper<Type> &> : _value<Type, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _make_value<_xvalue_wrapper<Type>> : _value<Type &&, _value_category::_xvalue>
		{
		};

		template <class Type>
		struct _make_value<_xvalue_wrapper<Type> &> : _value<Type &&, _value_category::_xvalue>
		{
		};

			// _is_bindable // TODO review this

		template <class Param1Type>
		void _function_one_param(Param1Type);
		
		template <class ParamType, class ArgType, class = void>
		struct _is_bindable : std::false_type
		{
		};

		template <class ParamType, class ArgType>
		struct _is_bindable<ParamType, ArgType, std::enable_if_t<std::is_placeholder_v<ArgType>>> : std::true_type
		{
		};

		template <class ParamType, class ArgType>
		struct _is_bindable<ParamType, ArgType, std::void_t<decltype(_function_one_param<ParamType>(ArgType()))>> : std::true_type
		{
		};

		template <class ... ParamTypes, class ... ArgTypes>
		struct _is_bindable<meta::pack<ParamTypes ...>, meta::pack<ArgTypes ...>, void> : std::conjunction<_is_bindable<ParamTypes, ArgTypes> ...>
		{
		};

			// _is_invocable_function_ptr_or_ref

		template <class FuncType, class = void>
		struct _is_invocable_function_ptr_or_ref_base : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class FuncType>
		struct _is_invocable_function_ptr_or_ref_base<FuncType, std::enable_if_t<std::is_function_v<FuncType>>> : std::true_type
		{
			using parameter_types = typename meta::function_signature<FuncType>::parameter_types;
		};

		template <class FuncType>
		struct _is_invocable_function_ptr_or_ref : _is_invocable_function_ptr_or_ref_base<std::remove_pointer_t<std::decay_t<FuncType>>>
		{
		};

			// _is_invocable_member_function_or_member_object_ptr

		template <class, class = void>
		struct _is_invocable_member_function_or_member_object_ptr_base : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class ObjType>
		struct _is_invocable_member_function_or_member_object_ptr_base<ObjType, std::enable_if_t<std::is_member_object_pointer_v<ObjType>>> : std::true_type
		{
			using parameter_types = meta::pack<>;
		};

		template <class FuncType>
		struct _is_invocable_member_function_or_member_object_ptr_base<FuncType, std::enable_if_t<std::is_member_function_pointer_v<FuncType>>> : std::true_type
		{
			using parameter_types = typename meta::function_signature<FuncType>::parameter_types;
		};

		template <class, class>
		struct _is_invocable_member_function_or_member_object_ptr : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class FuncObjType, class ClassType>
		struct _is_invocable_member_function_or_member_object_ptr<FuncObjType std::remove_pointer_t<std::decay_t<ClassType>>::*, ClassType> : _is_invocable_member_function_or_member_object_ptr_base<FuncObjType std::remove_pointer_t<std::decay_t<ClassType>>::*>
		{
		};

			// _is_invocable_function_object

		template <class, class = void>
		struct _is_invocable_function_object_base : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class ObjType>
		struct _is_invocable_function_object_base<ObjType, std::void_t<std::enable_if_t<std::is_object_v<ObjType>>, decltype(&ObjType::operator())>> : std::true_type
		{
			using parameter_types = typename meta::function_signature<decltype(&ObjType::operator())>::parameter_types;
		};

		template <class ObjType>
		struct _is_invocable_function_object : _is_invocable_function_object_base<std::remove_pointer_t<std::decay_t<ObjType>>>
		{
		};

			// _is_invocable

		template <class FuncType, class ... ArgTypes>
		struct _is_invocable_base
		{
			using is_invocable_function_ptr_or_ref = _is_invocable_function_ptr_or_ref<FuncType>;
			using is_invocable_member_function_or_member_object_ptr = _is_invocable_member_function_or_member_object_ptr<FuncType, typename meta::pack<ArgTypes ...>::template push<void>::first>;
			using is_invocable_function_object = _is_invocable_function_object<FuncType>;

			using parameter_types = 
				typename meta::type_if<is_invocable_function_ptr_or_ref::value>::template then<
					typename is_invocable_function_ptr_or_ref::parameter_types
				>::template else_if<is_invocable_member_function_or_member_object_ptr::value>::template then<
					typename is_invocable_member_function_or_member_object_ptr::parameter_types
				>::template else_if<is_invocable_function_object::value>::template then<
					typename is_invocable_function_object::parameter_types
				>::template else_then<
					meta::_implementation::_undefined
				>::end_if;
			using argument_types =
				typename meta::type_if<is_invocable_function_ptr_or_ref::value || is_invocable_function_object::value>::template then<
					meta::pack<ArgTypes ...>
				>::template else_if<is_invocable_member_function_or_member_object_ptr::value>::template then<
					meta::pack<ArgTypes ...>::template pop<1>
				>::template else_then<
					meta::_implementation::_undefined
				>::end_if;

			using _value_type = typename _is_bindable<parameter_types, argument_types>::value_type;
		};

		template <class FuncType, class ... ArgTypes>
		struct _is_invocable : _is_invocable_base<FuncType, ArgTypes ...>::_value_type
		{	
		};
	}

	//	TODO: implement own bind - ideas:
	//		- try to provide an interface similar to std::bind so it can be used with other standard types (function, packaged_task, etc...)
	//		- allow usage of tag types to change argument passing within binder object (see functional_bind_semantics.txt)
	//		- use internal block to store arguments (store arguments with special type that abstracts reference or actual object, as well as function call semantics)
	//		- provide semantics for nested/curried bind calls
	//		- provide better defined semantics for std::placeholders
	//		- provide re-bind/re-invoke semantics

		// _lvalue_wrapper

	template <class Type>
	class _lvalue_wrapper final : public _implementation::_value_category_wrapper<Type>
	{
	public:
		_lvalue_wrapper(Type & object) noexcept
			: _implementation::_value_category_wrapper<Type>(object)
		{
		}
	};

		// _xvalue_wrapper

	template <class Type>
	class _xvalue_wrapper final : public _implementation::_value_category_wrapper<Type>
	{
	public:
		_xvalue_wrapper(Type & object) noexcept
			: _implementation::_value_category_wrapper<Type>(object)
		{
		}
	};

	class _binder_internal
	{

	};

	template <class FuncType, class ... ArgTypes>
	class _binder_internal_argument : public _binder_internal_argument<ArgTypes ...>
	{
		
	};

	template <class FuncType, class ... ArgTypes>
	class _binder_internal_function : public _binder_internal_argument<ArgTypes ...>
	{
		
	};

	// TODO
//	template <class InvokeType, class FuncType, class ArgTypes ...> // this may be an non-instantiable class template type
	template <class RetType, class ... ValTypes> // TODO old idea, remove it
	class _binder
	{
	/*	using signature = meta::function_signature<FuncType>;
		using function_type =
			meta::make_function_signature<
				typename signature::return_type, 
				meta::transformed_pack<
					meta::apply_to_pack_element<meta::type_identity>::last, 
					meta::constrained_pack<
						meta::apply_to_pack_element<std::is_placeholder>::first, 
						meta::transposed_pack<
							meta::pack<typename ValTypes::type ...>, 
							typename signature::parameter_types
						>
					>
				>, 
				meta::pack<>
			>; */

	public:
		template <class FuncType, class ... ArgTypes>
		_binder(FuncType && func, ArgTypes && ... args)
		{
			func, void();
			((args, void()), ...);
		}

		template <class ... ParamTypes>
		RetType operator()(ParamTypes && ... params)
		{
			((params, void()), ...);
			return RetType(); // TODO remove this
		}

		std::shared_ptr<_binder_internal> _binder_ptr;
	};

	template <class Type>
	_lvalue_wrapper<Type> wrap_copy(Type & object) // TODO unsure if final name (alternative: bind_copy)
	{
	//	static_assert(!is_value_category_wrapper);
		return _lvalue_wrapper<Type>(object);
	}

	template <class Type>
	_lvalue_wrapper<Type> & wrap_copy(_lvalue_wrapper<Type> & object)
	{
		return object;
	}

	template <class Type>
	_xvalue_wrapper<Type> wrap_move(Type & object) // TODO unsure if final name (alternative: bind_move)
	{
	//	static_assert(!is_value_category_wrapper);
		return _xvalue_wrapper<Type>(object);
	}

	template <class Type>
	_xvalue_wrapper<Type> & wrap_move(_xvalue_wrapper<Type> & object)
	{
		return object;
	}

	template <class FuncType, class ... ArgTypes>
	auto xbind(FuncType && func, ArgTypes && ... args) // TODO temporary name, replace with "bind" once done
	{
		func, void();
		((args, void()), ...);
		// TODO 
	//	using semantics = _implementation::_bind_semantics<FuncType, ArgTypes...>; // This type will have a big part of the metaprogramming implementation
	//	static_assert(semantics::is_invocable, "TODO"); // Check example above for how to produce 'Informative error messages'
	//	static_assert(semantics::is_bindable, "TODO");
	//	return _binder<semantics::invocation_semantics>(_implementation::_bind_function<semantics::function_semantics>(std::forward<FuncType>(func)), _implementation::_bind_argument<semantics::argument_semantics>(std::forward<ArgTypes>(args)) ...);

		// TODO old idea, remove it
	//	static_assert(_implementation::_is_invocable<FuncType, typename _implementation::_make_value<ArgTypes>::type ...>::value, ""); // TODO enable this and create error message
		return _binder<typename meta::function_signature<FuncType>::return_type, _implementation::_make_value<ArgTypes> ...>(std::forward<FuncType>(func), std::forward<typename _implementation::_make_value<ArgTypes>::type>(args) ...);
	//	return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	template <class Type>
//	[[deprecated("Will be superseded by stdx::functional::xbind")]]
	auto forward(std::remove_reference_t<Type> & value) // Forwarding functions used with std::bind to pass stored arguments to the stored function with the correct semantics
	{
		if constexpr (std::is_placeholder_v<std::remove_cv_t<std::remove_reference_t<Type>>>)
		{
			return value;
		}
		else if constexpr (std::is_lvalue_reference_v<Type>)
		{
			return std::ref(value);
		}
		else
		{
			return std::bind(std::move<Type &>, std::move(value)); // Ideally, this would pass a std::ref(value) to the inner bind, but we can't be sure that whatever value reached here is REALLY an lvalue
		}
	}
	template <class Type>
//	[[deprecated("Will be superseded by stdx::functional::xbind")]]
	auto forward(std::remove_reference_t<Type> && value)
	{
		if constexpr (std::is_placeholder_v<std::remove_cv_t<std::remove_reference_t<Type>>>)
		{
			return value;
		}
		else
		{
			return std::bind(std::move<Type &>, std::move(value));
		}
	}

	template <class FuncType, class ... ArgTypes> // Wrapper for std::bind that uses stdx::functional::forward to forward the arguments to the function
//	[[deprecated("Will be superseded by stdx::functional::xbind")]]
	auto bind(FuncType * func, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, stdx::functional::forward<ArgTypes>(args) ...);
	}
	template <class FuncType, class ObjType, class ... ArgTypes>
//	[[deprecated("Will be superseded by stdx::functional::xbind")]]
	auto bind(FuncType ObjType::* func, ObjType * obj, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType ObjType::*, ObjType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, obj, stdx::functional::forward<ArgTypes>(args) ...);
	}
}
}

namespace std
{
	template <class RetType, class ... ValTypes>
	struct is_bind_expression<stdx::functional::_binder<RetType, ValTypes ...>> : std::true_type
	{
	};
}

#endif
