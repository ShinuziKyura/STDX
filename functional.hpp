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
	namespace _implementation
	{
	
	///////////////////////
	/// _value_category
	///////////////////////
		
		namespace _value_category
		{
			struct _lvalue;
			struct _xvalue;
			struct _prvalue;
		}

	///////////////////
	/// _expression
	///////////////////

		template <class Type, class Category>
		struct _expression // TODO unsure if necessary
		{
			using type = Type;
			using value_category = Category;
		};

	///////////////////////////////
	/// _value_category_wrapper
	///////////////////////////////

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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _invoke_semantics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////
	/// _invoke_semantics_class_object
	//////////////////////////////////////

		template <class, class T = void>
		struct _invoke_semantics_class_object
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "
												 "When FuncType is a reference or pointer to class object or a pointer to member class object, "
												 "it must have an accessible non-overloaded operator().");

			using _function_type = void;

			static constexpr bool _is_invocable = false;
		};

		template <class ClassType>
		struct _invoke_semantics_class_object<ClassType, std::void_t<decltype(&ClassType::operator())>>
		{
			using _function_type = meta::function_signature<decltype(&ClassType::operator())>;

			static constexpr bool _is_invocable = true;
		};

	/////////////////////////
	/// _invoke_semantics
	/////////////////////////

		template <class FuncType, class T = void>
		struct _invoke_semantics
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "
												 "FuncType must be either "
												 "a reference or pointer to function, "
												 "a pointer to member function, "
												 "a reference or pointer to class object with an accessible non-overloaded operator(), "
												 "or a pointer to member class object with an accessible non-overloaded operator().");

			using class_type = void;

			using function_type = void;

			static constexpr bool is_invocable = false;
		};

		template <class FuncType>
		struct _invoke_semantics<FuncType, std::enable_if_t<std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>>
		{
			using class_type = void; // Doesn't need object

			using function_type = meta::function_signature<std::remove_pointer_t<std::remove_reference_t<FuncType>>>;

			static constexpr bool is_invocable = true;
		};

		template <class FuncType>
		struct _invoke_semantics<FuncType, std::enable_if_t<std::is_class_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>>
		{
			using class_type = void;

			using function_type = typename _invoke_semantics_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_function_type;

			static constexpr bool is_invocable = _invoke_semantics_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_is_invocable;
		};

		template <class FuncType, class ClassType>
		struct _invoke_semantics<FuncType ClassType::*, std::enable_if_t<std::is_function_v<FuncType>>>
		{
			using class_type = ClassType; // Needs object (tip: static_assert(std::is_base_of_v<invoke_semantics::class_type, FirstArgType>))

			using function_type = meta::function_signature<FuncType>;

			static constexpr bool is_invocable = true;
		};

		template <class FuncType, class ClassType>
		struct _invoke_semantics<FuncType ClassType::*, std::enable_if_t<std::is_class_v<FuncType>>>
		{
			using class_type = ClassType;

			using function_type = typename _invoke_semantics_class_object<FuncType>::_function_type;

			static constexpr bool is_invocable = _invoke_semantics_class_object<FuncType>::_is_invocable;
		};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _bind_semantics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		// TODO add class name comment and add logic (tip: use meta::permutated_pack or variant for ParamPackType, use std::greater when inserting, and then reverse_valpack at the end before comparing with generated integer sequence)

	/*	template <class UnboundParamPackType, class ParamPackType, class ArgPackType, class OrderedPlaceholderPackType, class PlaceholderPackType, class T = void>
		struct _bind_semantics_smth
		{
			static_assert(meta::always_false<T>, ""); // TODO

			static constexpr bool _value = false;
		};

		template <int ... PlaceholderIndexValues>
		struct _bind_semantics_smth<meta::valpack<PlaceholderIndexValues ...>, std::enable_if_t<std::is_same_v<meta::make_integer_sequence<int, 1, sizeof...(PlaceholderIndexValues)>, std::integer_sequence<int, PlaceholderIndexValues ...>>>>
		{
			static constexpr bool _value = true;
		}; */

	//////////////////////////////
	/// _bind_semantics_arguments
	//////////////////////////////

		template <class ParamType>
		void _is_bindable_param(ParamType) noexcept;

		template <class ArgType>
		std::conditional_t<std::is_destructible_v<ArgType>, ArgType, std::add_rvalue_reference_t<ArgType>> _is_bindable_arg() noexcept;

		template <class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_arguments
		{
			static_assert(meta::always_false<T>, ""); // TODO

			static constexpr bool _is_bindable = false;
		};

		template <class ParamPackType, class ArgPackType>
		struct _bind_semantics_arguments<ParamPackType, ArgPackType, std::enable_if_t<std::is_placeholder_v<typename ArgPackType::front>>>
		{
			static constexpr bool _is_bindable = _bind_semantics_arguments<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>>::_is_bindable;
		};

		template <class ParamPackType, class ArgPackType>
		struct _bind_semantics_arguments<ParamPackType, ArgPackType, std::void_t<decltype(_is_bindable_param<typename ParamPackType::front>(_is_bindable_arg<typename ArgPackType::front>()))>>
		{
			static constexpr bool _is_bindable = _bind_semantics_arguments<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>>::_is_bindable;
		};

		template <>
		struct _bind_semantics_arguments<meta::valpack<>, meta::valpack<>, void>
		{
			static constexpr bool _is_bindable = true;
		};

	//////////////////////////////////////
	/// _bind_semantics_object_rvalue_ref
	//////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_object_rvalue_ref
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_object_rvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!QualSpecSeqType::is_rvalue_ref || (std::is_rvalue_reference_v<ObjType> && !std::is_pointer_v<std::remove_reference_t<ObjType>>)>>
		{
			static constexpr bool _is_bindable = _bind_semantics_arguments<ParamPackType, ArgPackType>::_is_bindable;
		};

	//////////////////////////////////////
	/// _bind_semantics_object_lvalue_ref
	//////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_object_lvalue_ref
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_object_lvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!QualSpecSeqType::is_lvalue_ref || (std::is_lvalue_reference_v<ObjType> || std::is_pointer_v<std::remove_reference_t<ObjType>>)>>
		{
			static constexpr bool _is_bindable = _bind_semantics_object_rvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	////////////////////////////////////
	/// _bind_semantics_object_volatile
	////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_object_volatile
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_object_volatile<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!std::is_volatile_v<meta::fundamental_type<ObjType>> || QualSpecSeqType::is_volatile>>
		{
			static constexpr bool _is_bindable = _bind_semantics_object_lvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	/////////////////////////////////
	/// _bind_semantics_object_const
	/////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_object_const
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;
			
		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_object_const<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!std::is_const_v<meta::fundamental_type<ObjType>> || QualSpecSeqType::is_const>>
		{
			static constexpr bool _is_bindable = _bind_semantics_object_volatile<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	/////////////////////////////////
	/// _bind_semantics_object_class
	/////////////////////////////////

		template <class ClassType, class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_object_class
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;
		};

		template <class ClassType, class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_object_class<ClassType, ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<std::is_base_of_v<ClassType, meta::fundamental_type<ObjType>>>>
		{
			static constexpr bool _is_bindable = _bind_semantics_object_const<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	///////////////////////
	/// _bind_semantics
	///////////////////////

		template <class InvokeSemantics, class ArgPackType, class T = void>
		struct _bind_semantics
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO non-matching num of args

			using object_type = void;

			static constexpr bool is_bindable = false;
		};

		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<std::is_void_v<typename InvokeSemantics::class_type> && InvokeSemantics::function_type::parameter_types::size == ArgPackType::size>>
		{
			using object_type = void;

			static constexpr bool is_bindable = _bind_semantics_arguments<
				typename InvokeSemantics::function_type::parameter_types,
				ArgPackType
			>::_is_bindable;
		};

		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<std::is_class_v<typename InvokeSemantics::class_type> && InvokeSemantics::function_type::parameter_types::size + 1 == ArgPackType::size>>
		{
			using object_type = typename ArgPackType::first;

//			using invoke_type = _bind_semantics_smth<meta::pack<>, typename InvokeSemantics::function_type::parameter_types, ArgPackType, meta::valpack<>, meta::valpack<>> // TODO get better name

			static constexpr bool is_bindable = _bind_semantics_object_class<
				typename InvokeSemantics::class_type,
				typename ArgPackType::first,
				typename InvokeSemantics::function_type::qss_type,
				typename InvokeSemantics::function_type::parameter_types,
				typename ArgPackType::template pop<1>
			>::_is_bindable;
		};


		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<!InvokeSemantics::is_invocable>>
		{
			using object_type = void;

			static constexpr bool is_bindable = true; // Special case to prevent triggering the is_bindable assertions when the failure happens at is_invocable
		};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _binder_semantics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/////////////////////////
	/// _binder_semantics
	/////////////////////////

		template <class FuncType, class ... ArgTypes>
		struct _binder_semantics
		{
			using invoke_semantics = _invoke_semantics<FuncType>;
			using bind_semantics = _bind_semantics<invoke_semantics, meta::pack<ArgTypes ...>>;

			static constexpr bool is_invocable = invoke_semantics::is_invocable;
			static constexpr bool is_bindable = bind_semantics::is_bindable; // TODO WIP
		};

	}

	//	TODO: implement own bind - ideas:
	//		- try to provide an interface similar to std::bind so it can be used with other standard types (function, packaged_task, etc...)
	//		- allow usage of tag types to change argument passing within binder object (see functional_bind_semantics.txt)
	//		- use internal block to store arguments (store arguments with special type that abstracts reference or actual object, as well as function call semantics)
	//		- provide semantics for nested/curried bind calls
	//		- provide better defined semantics for std::placeholders
	//		- provide re-bind/re-invoke semantics

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _bind
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO this will hold most of the implementation
	template <class InvokeType, class FuncType, class ... ArgTypes> // this may be an non-instantiable class template type
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
	};

	template <class FuncType, class ... ArgTypes>
	auto xbind(FuncType && func, ArgTypes && ... args) // TODO temporary name, replace with "bind" once done
	{
		func, void();
		((args, void()), ...);
		// TODO 
		using semantics = _implementation::_binder_semantics<std::add_rvalue_reference_t<FuncType>, std::add_rvalue_reference_t<ArgTypes> ...>; // This type will have a big part of the metaprogramming implementation
		static_assert(semantics::is_invocable, "'stdx::bind(FuncType, ArgTypes ...)': FuncType must be invocable.");
		static_assert(semantics::is_bindable, "TODO is_bindable"); // TODO
	//	return _binder<semantics::bind_semantics>(_implementation::_bind_function<semantics::function_semantics>(std::forward<FuncType>(func)), _implementation::_bind_argument<semantics::argument_semantics>(std::forward<ArgTypes>(args)) ...); // Smth like this

		return 0;
	}

	// TODO update impl

/*	template <class Type>
	_prvalue_wrapper<Type> wrap_copy(Type & object) // TODO unsure if final name (alternative: bind_copy)
	{
	//	static_assert(!is_value_category_wrapper);
		return _prvalue_wrapper<Type>(object);
	}

	template <class Type>
	_prvalue_wrapper<Type> & wrap_copy(_prvalue_wrapper<Type> & object)
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
	} */

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
	// TODO FIXME once implementation is finished
/*	template <class RetType, class ... ValTypes>
	struct is_bind_expression<stdx::functional::_binder<RetType, ValTypes ...>> : std::true_type
	{
	}; */
}

#endif
