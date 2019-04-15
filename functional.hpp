#ifndef STDX_header_FUNCTIONAL
#define STDX_header_FUNCTIONAL

#include <functional>
#include "meta.hpp"

namespace stdx::functional
{
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

	template <class FuncType, class = void>
	struct _is_invocable_function : std::false_type
	{
	};

	template <class FuncType>
	struct _is_invocable_function<FuncType, std::enable_if_t<std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>> : std::true_type
	{
		using signature = meta::function_signature<FuncType>;
	};

	template <class FuncType>
	struct _is_invocable_function_ptr_or_ref : _is_invocable_function<FuncType>
	{
	};

	template <class FuncType, class = void>
	struct _is_invocable_member_function_ptr : std::true_type
	{
	};

	template <class FuncType>
	struct _is_invocable_member_function_ptr<FuncType, std::enable_if_t<std::is_member_function_pointer_v<FuncType>>> : std::true_type
	{
		using signature = meta::function_signature<FuncType>;
	};

	template <class, class ...>
	struct _is_invocable_member_function_or_member_object_ptr : std::false_type
	{
		static constexpr bool is_function = false;
		static constexpr bool is_object = false;
	};

	template <class FuncObjType, class ObjType, class ... ArgTypes>
	struct _is_invocable_member_function_or_member_object_ptr<FuncObjType ObjType::*, ArgTypes ...> : _is_invocable_member_function_ptr<FuncObjType ObjType::*>
	{
		static constexpr bool is_function = std::is_function_v<FuncObjType>;
		static constexpr bool is_object = std::is_object_v<FuncObjType>;
	};

	template <class, class = void>
	struct _has_function_call_operator : std::false_type
	{
	};

	template <class ObjType>
	struct _has_function_call_operator<ObjType, std::void_t<decltype(&ObjType::operator())>> : std::true_type
	{
		using signature = meta::function_signature<decltype(&ObjType::operator())>;
	};

	template <class FuncObjType>
	struct _is_invocable_function_object : std::conjunction<std::is_object<std::decay_t<FuncObjType>>, _has_function_call_operator<std::decay_t<FuncObjType>>>
	{
	};

	template <class FuncType, class ... ArgTypes>
	struct _is_invocable
	{
		using _is_invocable_function_ptr_or_ref_type = _is_invocable_function_ptr_or_ref<FuncType>;
		using _is_invocable_member_function_or_member_object_ptr_type = _is_invocable_member_function_or_member_object_ptr<FuncType, ArgTypes ...>;
		using _is_invocable_function_object_type = _is_invocable_function_object<FuncType>;

		static constexpr bool value = 
			_is_invocable_function_ptr_or_ref_type::value 
			|| _is_invocable_member_function_or_member_object_ptr_type::value 
			|| _is_invocable_function_object_type::value;
	};

	template <class FuncType, class ... ArgTypes>
	auto bind_WIP(FuncType && func, ArgTypes && ... args)
	{
		static_assert(_is_invocable<FuncType, ArgTypes ...>::value);
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
