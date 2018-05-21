#ifndef BINDER_HPP
#define BINDER_HPP

#include <functional>

#include "templates.hpp"

// https://stackoverflow.com/questions/27566894/how-to-check-if-a-type-is-defined-by-typedef-or-using-in-template-parameters?rq=1

namespace stdx::binder
{
	template <class FuncType, class ... ArgTypes>
	auto bind(FuncType * func, ArgTypes && ... args)
	{
		static_assert(sizeof(stdx::templates::function_signature<FuncType>), 
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, bind_forward<ArgTypes>(args) ...);
	}
	template <class FuncType, class ObjType, class ... ArgTypes>
	auto bind(FuncType ObjType::* func, ObjType * obj, ArgTypes && ... args)
	{
		static_assert(sizeof(stdx::templates::function_signature<FuncType>),
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, obj, bind_forward<ArgTypes>(args) ...);
	}

	template <class ValType>
	auto bind_forward(std::remove_reference_t<ValType> & val)
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
	auto bind_forward(std::remove_reference_t<ValType> && val)
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
}

#endif
