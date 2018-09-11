#ifndef STDX_FUNCTIONAL_HPP
#define STDX_FUNCTIONAL_HPP

#include <functional>

namespace stdx::functional
{
	template <class ValType>
	auto forward(std::remove_reference_t<ValType> & val) // Special forwarding functions to be used with bind
	{
		if constexpr (std::is_placeholder_v<std::remove_cv_t<std::remove_reference_t<ValType>>>)
		{
			return val;
		}
		else if constexpr (std::is_lvalue_reference_v<ValType>)
		{
			return std::ref(val);
		}
		else
		{
			return std::bind(std::move<ValType &>, std::move(val));
		}
	}
	template <class ValType>
	auto forward(std::remove_reference_t<ValType> && val)
	{
		if constexpr (std::is_placeholder_v<std::remove_cv_t<std::remove_reference_t<ValType>>>)
		{
			return val;
		}
		else
		{
			return std::bind(std::move<ValType &>, std::move(val));
		}
	}

	template <class FuncType, class ... ArgTypes> // Version of bind that respects argument passing semantics, actually a wrapper for std::bind using stdx::functional::forward
	auto bind(FuncType * func, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, forward<ArgTypes>(args) ...);
	}
	template <class FuncType, class ObjType, class ... ArgTypes>
	auto bind(FuncType ObjType::* func, ObjType * obj, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::functional::bind(FuncType ObjType::*, ObjType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, obj, forward<ArgTypes>(args) ...);
	}
}

#if defined(STDX_USING_FUNCTIONAL) || defined(STDX_USING_ALL)
namespace stdx { using namespace functional; }
#endif

#endif
