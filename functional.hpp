#ifndef STDX_header_FUNCTIONAL
#define STDX_header_FUNCTIONAL

#include <functional>

#include "meta.hpp"

namespace stdx::functional
{
	namespace _implementation
	{
			// _is_bindable

		template <class ParamTypes, class ArgTypes, class = void>
		struct _is_bindable : std::false_type
		{
		};

		template <class ParamTypes, class ArgTypes>
		struct _is_bindable<ParamTypes, ArgTypes, std::enable_if_t<std::is_convertible_v<typename ArgTypes::first, typename ParamTypes::first> || std::is_placeholder_v<typename ArgTypes::first>>>
			: _is_bindable<typename ParamTypes::template pop<1>, typename ArgTypes::template pop<1>>
		{
		};

		template <>
		struct _is_bindable<meta::pack<>, meta::pack<>, void> : std::true_type
		{
		};

			// _is_invocable_function_ptr_or_ref

		template <class FuncType, class = void>
		struct _is_invocable_function_ptr_or_ref_base : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class FuncType>
		struct _is_invocable_function_ptr_or_ref_base<FuncType, std::enable_if_t<std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>> : std::true_type
		{
			using parameter_types = typename meta::function_signature<FuncType>::parameter_types;
		};

		template <class FuncType>
		struct _is_invocable_function_ptr_or_ref : _is_invocable_function_ptr_or_ref_base<FuncType>
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

		template <class, class ...>
		struct _is_invocable_member_function_or_member_object_ptr : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class FuncObjType, class ClassType, class ... ArgTypes>
		struct _is_invocable_member_function_or_member_object_ptr<FuncObjType std::decay_t<ClassType>::*, ClassType, ArgTypes ...> : _is_invocable_member_function_or_member_object_ptr_base<FuncObjType std::decay_t<ClassType>::*>
		{
		};

			// _is_invocable_function_object

		template <class, class = void>
		struct _is_invocable_function_object_base : std::false_type
		{
			using parameter_types = meta::_implementation::_undefined;
		};

		template <class FuncObjType>
		struct _is_invocable_function_object_base<FuncObjType, std::void_t<std::enable_if_t<std::is_object_v<FuncObjType>>, decltype(&FuncObjType::operator())>> : std::true_type
		{
			using parameter_types = typename meta::function_signature<decltype(&FuncObjType::operator())>::parameter_types;
		};

		template <class FuncObjType>
		struct _is_invocable_function_object : _is_invocable_function_object_base<std::decay_t<FuncObjType>>
		{
		};

			// _is_invocable

		template <class FuncType, class ... ArgTypes>
		struct _is_invocable 
			: std::bool_constant<
				meta::value_if<_is_invocable_function_ptr_or_ref<FuncType>::value>::template then<
					_is_bindable<typename _is_invocable_function_ptr_or_ref<FuncType>::parameter_types, meta::pack<ArgTypes ...>>::value
				>::template else_if<_is_invocable_member_function_or_member_object_ptr<FuncType, ArgTypes ...>::value>::template then<
					_is_bindable<typename _is_invocable_member_function_or_member_object_ptr<FuncType, ArgTypes ...>::parameter_types, typename meta::pack<ArgTypes ...>::template pop<1>>::value
				>::template else_if<_is_invocable_function_object<FuncType>::value>::template then<
					_is_bindable<typename _is_invocable_function_object<FuncType>::parameter_types, meta::pack<ArgTypes ...>>::value
				>::template else_then<
					false
				>::end_if	
			>
		{	
		};

		template <class ...>
		class _tuple_base
		{
			template <std::size_t Index>
			decltype(auto) get()
			{
				static_assert(false, "Invalid index!"); // TODO
				return 0;
			}
		};

		template <class Type, class ... Types>
		class _tuple_base<Type, Types ...> : public _tuple_base<Types ...>
		{
			using base_type = _tuple_base<Types ...>;
		public:
			template <std::size_t Index>
			decltype(auto) get()
			{
				if constexpr (Index == 0)
				{
					return (_element); // Parenthesis make this an lvalue-reference
				}
				else
				{
					return base_type::template get<Index - 1>();
				}
			}

		private:
			Type _element;
		};

		template <class ... Types>
		class tuple : public _tuple_base<Types ...>
		{
			using base_type = _tuple_base<Types ...>;
		public:
			template <std::size_t Index>
			decltype(auto) get()
			{
				return base_type::template get<Index>();
			}
		};
	}

	template <class Type>
	struct _movable_t // TODO get a better name
	{
		using type = Type; // TODO add stuff to interface as necessary

		_movable_t(Type && object)
			: _object(object)
		{
		}

		Type && _object;

	};

	template <class Type>
	struct _is_movable_t : std::false_type
	{
	};

	template <class Type>
	struct _is_movable_t<_movable_t<Type>> : std::true_type
	{
	};

	// TODO _is_movable_t convenience type alias

	template <class Type>
	_movable_t<Type> bind_move(std::remove_reference_t<Type> & object)
	{
		// TODO disallow nested calls - make it transparent to the user and return object if it's already of type _movable_t<T>
		return _movable_t<Type>(object); // TODO review this
	}

	//	TODO: implement own bind - ideas:
	//		- allow usage of tag types to optimize argument passing within binder object
	//			- if ArgType is of non-reference type, move/copy it to object, and move/copy it to function
	//			- if ArgType is of lvalue-reference type, store reference inside object, and pass it to function
	//			- if ArgType is of rvalue-reference type, move it to object by default, store reference inside object if tagged, and move it to function
	//		- use internal block to store arguments, similarly to std::shared_ptr's reference counter
	//		- try to provide an interface similar to std::bind so it can be used with other standard types (function, packaged_task, etc...)
	//		- provide better defined semantics for std::placeholders
	//		- provide semantics for nested/curried bind calls
	//		- provide re-bind/re-invoke semantics

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
	auto bind(FuncType * func, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, stdx::functional::forward<ArgTypes>(args) ...);
	}
	template <class FuncType, class ObjType, class ... ArgTypes>
	auto bind(FuncType ObjType::* func, ObjType * obj, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType ObjType::*, ObjType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, obj, stdx::functional::forward<ArgTypes>(args) ...);
	}

	template <class FuncType, class ... ArgTypes>
	auto bind_WIP([[maybe_unused]] FuncType && func, ArgTypes && ... args)
	{
		((args, void()), ...);
		static_assert(_implementation::_is_invocable<FuncType, ArgTypes ...>::value, "TODO"); // TODO
	}
}

#endif

//=====

#if defined(STDX_USING_FUNCTIONAL) || defined(STDX_USING_ALL)

namespace stdx 
{ 
	using namespace functional;
}

#endif
