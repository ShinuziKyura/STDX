#ifndef STDX_header_FUNCTIONAL
#define STDX_header_FUNCTIONAL

#include <functional>

#include "meta.hpp"

namespace stdx
{
#if !(defined(STDX_NAMESPACE_FUNCTIONAL) || defined(STDX_NAMESPACE_ALL))
inline
#endif
namespace functional
{
	template <class>
	class _lvalue_binder;

	template <class>
	class _xvalue_binder;

	namespace _implementation
	{
		namespace _value_category
		{
			struct _rvalue;
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

			// _value_category_binder

		template <class Type>
		class _value_category_binder
		{
		public:
			_value_category_binder(Type & object) noexcept
				: _object(object)
			{
			}
			_value_category_binder(_value_category_binder const &) = delete;
			_value_category_binder(_value_category_binder && other) noexcept
				: _object(other._object)
			{
			}
			_value_category_binder & operator=(_value_category_binder) = delete;

			operator Type &()
			{
				return _object;
			}

		private:
			Type & _object;

		};

			// _bind_value_category

		template <class Type>
		struct _bind_value_category : _value<Type, _value_category::_rvalue>
		{
		};

		template <class Type>
		struct _bind_value_category<Type &> : _value<Type &, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _bind_value_category<_lvalue_binder<Type>> : _value<Type, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _bind_value_category<_lvalue_binder<Type> &> : _value<Type, _value_category::_lvalue>
		{
		};

		template <class Type>
		struct _bind_value_category<_xvalue_binder<Type>> : _value<Type &&, _value_category::_xvalue>
		{
		};

		template <class Type>
		struct _bind_value_category<_xvalue_binder<Type> &> : _value<Type &&, _value_category::_xvalue>
		{
		};

			// _is_bindable

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
	//		- allow usage of tag types to optimize argument passing within binder object
	//			- if ArgType is of non-reference type, move/copy it to object, and move/copy it to function
	//			- if ArgType is of lvalue-reference type, store reference inside object by default, copy it to object if tagged, and pass it to function
	//			- if ArgType is of rvalue-reference type, move it to object by default, store reference inside object if tagged, and move it to function
	//		- use internal block to store arguments
	//		- provide semantics for nested/curried bind calls
	//		- provide better defined semantics for std::placeholders
	//		- provide re-bind/re-invoke semantics

		// _lvalue_binder

	template <class Type>
	class _lvalue_binder final : public _implementation::_value_category_binder<Type>
	{
	public:
		_lvalue_binder(Type & object) noexcept
			: _implementation::_value_category_binder<Type>(object)
		{
		}
	};

		// _xvalue_binder

	template <class Type>
	class _xvalue_binder final : public _implementation::_value_category_binder<Type>
	{
	public:
		_xvalue_binder(Type & object) noexcept
			: _implementation::_value_category_binder<Type>(object)
		{
		}
	};

	template <class FuncType, class ... ValTypes>
	class _binder
	{
		using signature = meta::function_signature<FuncType>;
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
			>;
	public:
		template <class ... ArgTypes>
		_binder(FuncType && func, ArgTypes && ... args)
			: _function(std::forward<FuncType>(func))
		{
			func, void();
			((args, void()), ...);
		}

		FuncType _function;
	};

	template <class Type>
	_lvalue_binder<Type> bind_copy(Type & object)
	{
		return _lvalue_binder<Type>(object);
	}

	template <class Type>
	_lvalue_binder<Type> & bind_copy(_lvalue_binder<Type> & object)
	{
		return object;
	}

	template <class Type>
	_lvalue_binder<Type> bind_copy(_lvalue_binder<Type> && object)
	{
		return std::move(object);
	}

	template <class Type>
	_xvalue_binder<Type> bind_move(Type & object)
	{
		return _xvalue_binder<Type>(object);
	}

	template <class Type>
	_xvalue_binder<Type> & bind_move(_xvalue_binder<Type> & object)
	{
		return object;
	}

	template <class Type>
	_xvalue_binder<Type> bind_move(_xvalue_binder<Type> && object)
	{
		return std::move(object);
	}

	template <class FuncType, class ... ArgTypes>
	auto xbind(FuncType && func, ArgTypes && ... args) // TODO temporary name, replace with "bind" once done
	{
		func, void();
		((args, void()), ...);
	//	static_assert(_implementation::_is_invocable<FuncType, typename _implementation::_bind_value_category<ArgTypes>::type ...>::value, ""); // TODO enable this and create error message
		return _binder<FuncType, _implementation::_bind_value_category<ArgTypes> ...>(std::forward<FuncType>(func), std::forward<typename _implementation::_bind_value_category<ArgTypes>::type>(args) ...);
	//	return 0;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////

	template <class Type>
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

#endif
