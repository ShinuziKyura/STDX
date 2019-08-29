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

	////////////////////////////////////////////
	/// _invoke_semantics_check_class_object
	////////////////////////////////////////////

		template <class, class T = void>
		struct _invoke_semantics_check_class_object
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "
												 "When FuncType is a reference or pointer to class object or a pointer to member class object, "
												 "it must have an accessible non-overloaded operator().");

			using _function_signature = void;

			static constexpr bool _is_invocable = false;
		};

		template <class ClassType>
		struct _invoke_semantics_check_class_object<ClassType, std::void_t<decltype(&ClassType::operator())>>
		{
			using _function_signature = meta::function_signature<decltype(&ClassType::operator())>;

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
			using function_signature = void;

			static constexpr bool is_invocable = false;
		};

		template <class FuncType>
		struct _invoke_semantics<FuncType, std::enable_if_t<std::is_function_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>>
		{
			using class_type = void;
			using function_signature = meta::function_signature<std::remove_pointer_t<std::remove_reference_t<FuncType>>>;

			static constexpr bool is_invocable = true;
		};

		template <class FuncType>
		struct _invoke_semantics<FuncType, std::enable_if_t<std::is_class_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>>
		{
			using class_type = void;
			using function_signature = typename _invoke_semantics_check_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_function_signature;

			static constexpr bool is_invocable = _invoke_semantics_check_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_is_invocable;
		};

		template <class FuncType, class ClassType>
		struct _invoke_semantics<FuncType ClassType::*, std::enable_if_t<std::is_function_v<FuncType>>>
		{
			using class_type = ClassType;
			using function_signature = meta::function_signature<FuncType>;

			static constexpr bool is_invocable = true;
		};

		template <class FuncType, class ClassType>
		struct _invoke_semantics<FuncType ClassType::*, std::enable_if_t<std::is_class_v<std::remove_pointer_t<std::remove_reference_t<FuncType>>>>>
		{
			using class_type = ClassType;
			using function_signature = typename _invoke_semantics_check_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_function_signature;

			static constexpr bool is_invocable = _invoke_semantics_check_class_object<std::remove_pointer_t<std::remove_reference_t<FuncType>>>::_is_invocable;
		};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _bind_semantics
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	////////////////////////////////////////
	/// _bind_semantics_check_parameters
	////////////////////////////////////////

		template <class ParamPackType, class ArgPackType, class PHParamPackType, class PHPermPackType, class = void>
		struct _bind_semantics_check_parameters
		{
			using _parameter_types = typename _bind_semantics_check_parameters<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>, PHParamPackType, PHPermPackType>::_parameter_types;
		};

		template <class ParamPackType, class ArgPackType, class PHParamPackType, class PHPermPackType>
		struct _bind_semantics_check_parameters<ParamPackType, ArgPackType, PHParamPackType, PHPermPackType, std::enable_if_t<std::is_placeholder_v<typename ArgPackType::front>>>
		{
			using _parameter_types = typename _bind_semantics_check_parameters<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>, typename PHParamPackType::template push<typename ParamPackType::front>, typename PHPermPackType::template push<std::is_placeholder_v<typename ArgPackType::front>>>::_parameter_types;
		};

		template <class ParamPackType, class PHParamPackType, class PHPermPackType>
		struct _bind_semantics_check_parameters<ParamPackType, meta::pack<>, PHParamPackType, PHPermPackType, void>
		{
			using _parameter_types = typename _bind_semantics_check_parameters<typename ParamPackType::template push<typename PHParamPackType::template pop<PHPermPackType::front - 1>::front>, meta::pack<>, PHParamPackType, typename PHPermPackType::template pop<1>>::_parameter_types;
		};

		template <class ParamPackType, class PHParamPackType>
		struct _bind_semantics_check_parameters<ParamPackType, meta::pack<>, PHParamPackType, meta::valpack<>, void>
		{
			using _parameter_types = ParamPackType;
		};

	//////////////////////////////////////////
	/// _bind_semantics_check_placeholders
	//////////////////////////////////////////
	
		template <class PHIndexPackType, class T = void>
		struct _bind_semantics_check_placeholders
		{
			static_assert(meta::always_false<T>, ""); // TODO

			static constexpr bool _is_bindable = false;
		};

		template <int ... PHIndexValues>
		struct _bind_semantics_check_placeholders<meta::valpack<PHIndexValues ...>, std::enable_if_t<std::is_same_v<meta::make_integer_sequence<int, 1, meta::valpack<PHIndexValues ...>::size>, std::integer_sequence<int, PHIndexValues ...>>>>
		{
			static constexpr bool _is_bindable = true;
		};

	///////////////////////////////////////
	/// _bind_semantics_check_arguments
	///////////////////////////////////////

		template <class ParamType>
		void _is_bindable_check_param(ParamType) noexcept;

		template <class ArgType>
		std::conditional_t<std::is_destructible_v<ArgType>, ArgType, std::add_rvalue_reference_t<ArgType>> _is_bindable_check_arg() noexcept;

		template <class ParamPackType, class ArgPackType, class PHIndexPackType, class T = void>
		struct _bind_semantics_check_arguments
		{
			static_assert(meta::always_false<T>, ""); // TODO

			static constexpr bool _is_bindable = false;
		};

		template <class ParamPackType, class ArgPackType, class PHIndexPackType>
		struct _bind_semantics_check_arguments<ParamPackType, ArgPackType, PHIndexPackType, std::enable_if_t<std::is_placeholder_v<typename ArgPackType::front>>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_arguments<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>, typename PHIndexPackType::template insert<std::is_placeholder_v<typename ArgPackType::front>, std::greater<>>>::_is_bindable;
		};

		template <class ParamPackType, class ArgPackType, class PHIndexPackType>
		struct _bind_semantics_check_arguments<ParamPackType, ArgPackType, PHIndexPackType, std::void_t<decltype(_is_bindable_check_param<typename ParamPackType::front>(_is_bindable_check_arg<typename ArgPackType::front>()))>> // TODO detect wrapper types and properly consider resulting type (according to functional_bind_semantics.text)
		{
			static constexpr bool _is_bindable = _bind_semantics_check_arguments<typename ParamPackType::template pop<1>, typename ArgPackType::template pop<1>, PHIndexPackType>::_is_bindable;
		};

		template <class PHIndexPackType>
		struct _bind_semantics_check_arguments<meta::pack<>, meta::pack<>, PHIndexPackType, void>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_placeholders<meta::reverse_valpack<PHIndexPackType>>::_is_bindable;
		};

	///////////////////////////////////////////////
	/// _bind_semantics_check_object_rvalue_ref
	///////////////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_check_object_rvalue_ref
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_check_object_rvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!QualSpecSeqType::is_rvalue_ref || (std::is_rvalue_reference_v<ObjType> && !std::is_pointer_v<std::remove_reference_t<ObjType>>)>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_arguments<ParamPackType, ArgPackType, meta::valpack<>>::_is_bindable;
		};

	///////////////////////////////////////////////
	/// _bind_semantics_check_object_lvalue_ref
	///////////////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_check_object_lvalue_ref
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_check_object_lvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!QualSpecSeqType::is_lvalue_ref || (std::is_lvalue_reference_v<ObjType> || std::is_pointer_v<std::remove_reference_t<ObjType>>)>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_object_rvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	/////////////////////////////////////////////
	/// _bind_semantics_check_object_volatile
	/////////////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_check_object_volatile
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;

		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_check_object_volatile<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!std::is_volatile_v<meta::fundamental_type<ObjType>> || QualSpecSeqType::is_volatile>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_object_lvalue_ref<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	//////////////////////////////////////////
	/// _bind_semantics_check_object_const
	//////////////////////////////////////////

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_check_object_const
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;
			
		};

		template <class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_check_object_const<ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<!std::is_const_v<meta::fundamental_type<ObjType>> || QualSpecSeqType::is_const>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_object_volatile<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	//////////////////////////////////////////
	/// _bind_semantics_check_object_class
	//////////////////////////////////////////

		template <class ClassType, class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType, class T = void>
		struct _bind_semantics_check_object_class
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO

			static constexpr bool _is_bindable = false;
		};

		template <class ClassType, class ObjType, class QualSpecSeqType, class ParamPackType, class ArgPackType>
		struct _bind_semantics_check_object_class<ClassType, ObjType, QualSpecSeqType, ParamPackType, ArgPackType, std::enable_if_t<std::is_base_of_v<ClassType, meta::fundamental_type<ObjType>>>>
		{
			static constexpr bool _is_bindable = _bind_semantics_check_object_const<ObjType, QualSpecSeqType, ParamPackType, ArgPackType>::_is_bindable;
		};

	///////////////////////
	/// _bind_semantics
	///////////////////////

		template <class InvokeSemantics, class ArgPackType, class T = void>
		struct _bind_semantics
		{
			static_assert(meta::always_false<T>, "'stdx::bind(FuncType, ArgTypes ...)': "); // TODO non-matching num of args

			using return_type = void;
			using object_type = void;
			using parameter_types = void;

			static constexpr bool is_bindable = false;
		};

		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<std::is_void_v<typename InvokeSemantics::class_type> && InvokeSemantics::function_signature::parameter_types::size == ArgPackType::size>>
		{
			using return_type = typename InvokeSemantics::function_signature::return_type;
			using object_type = void;
			using parameter_types = typename _bind_semantics_check_parameters<
				typename InvokeSemantics::function_signature::parameter_types,
				ArgPackType,
				meta::pack<>,
				meta::valpack<>
			>::_parameter_types;

			static constexpr bool is_bindable = _bind_semantics_check_arguments<
				typename InvokeSemantics::function_signature::parameter_types,
				ArgPackType,
				meta::valpack<>
			>::_is_bindable;
		};

		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<std::is_class_v<typename InvokeSemantics::class_type> && InvokeSemantics::function_signature::parameter_types::size + 1 == ArgPackType::size>>
		{
			using return_type = typename InvokeSemantics::function_signature::return_type;
			using object_type = typename ArgPackType::front;
			using parameter_types = typename _bind_semantics_check_parameters<
				typename InvokeSemantics::function_signature::parameter_types,
				typename ArgPackType::template pop<1>,
				meta::pack<>,
				meta::valpack<>
			>::_parameter_types;

			static constexpr bool is_bindable = _bind_semantics_check_object_class<
				typename InvokeSemantics::class_type,
				typename ArgPackType::front,
				typename InvokeSemantics::function_signature::qss_type,
				typename InvokeSemantics::function_signature::parameter_types,
				typename ArgPackType::template pop<1>
			>::_is_bindable;
		};


		template <class InvokeSemantics, class ArgPackType>
		struct _bind_semantics<InvokeSemantics, ArgPackType, std::enable_if_t<!InvokeSemantics::is_invocable>>
		{
			using return_type = void;
			using object_type = void;
			using parameter_types = void;

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
			static constexpr bool is_bindable = bind_semantics::is_bindable;

			using return_type = typename bind_semantics::return_type;
			using object_type = typename bind_semantics::object_type;
			using parameter_types = typename bind_semantics::parameter_types;
		};

	}

	//	TODO: implement own bind - ideas:
	//		- try to provide an interface similar to std::bind so it can be used with other standard types (function, packaged_task, etc...)
	//		- allow usage of tag types to change argument passing within binder object (see functional_bind_semantics.txt)
	//		- use internal block to store arguments (store arguments with special type that abstracts reference or actual object, as well as function call semantics)
	//		- provide better defined semantics for std::placeholders
	//		- provide re-bind/re-invoke semantics
	//		- provide semantics for nested/curried bind calls

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// _bind
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// TODO this will hold most of the implementation
	template <class InvokeType, class FuncType, class ... ArgTypes> // this may be an non-instantiable class template type
	class _binder
	{

	};

	template <class FuncType, class ... ArgTypes>
	auto xbind(FuncType && func, ArgTypes && ... args) // TODO temporary name, replace with "bind" once done
	{
		// TODO 
		// TODO unsure if FuncType should/needs to be wrapped with add_rval_ref in semantics, as it may be an object for which we may want to preserve value category
		using semantics = _implementation::_binder_semantics<FuncType, std::add_rvalue_reference_t<ArgTypes> ...>; // This type will have a big part of the metaprogramming implementation
		static_assert(semantics::is_invocable, "'stdx::bind(FuncType, ArgTypes ...)': FuncType must be invocable.");
		static_assert(semantics::is_bindable, "TODO is_bindable"); // TODO
	//	return _binder<semantics::bind_semantics>(_implementation::_bind_function<semantics::function_semantics>(std::forward<FuncType>(func)), _implementation::_bind_argument<semantics::argument_semantics>(std::forward<ArgTypes>(args)) ...); // Smth like this

		func, void();
		((args, void()), ...);
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
