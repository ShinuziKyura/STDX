#ifndef BINDER_HPP
#define BINDER_HPP

#include <functional>

namespace stdx::binder
{
	template <class FuncType, class ... ArgTypes>
	auto bind(FuncType * func, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::binder::bind(FuncType *, ArgTypes && ...)': "
					  "FuncType must be a function, a pointer to function, or a pointer to member function");
		return std::bind(func, bind_forward<ArgTypes>(args) ...);
	}
	template <class FuncType, class ObjType, class ... ArgTypes>
	auto bind(FuncType ObjType::* func, ObjType * obj, ArgTypes && ... args)
	{
		static_assert(std::is_function_v<FuncType>,
					  "'stdx::binder::bind(FuncType ObjType::*, ObjType *, ArgTypes && ...)': "
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

#if defined(STDX_USING_BINDER) || defined(STDX_USING_ALL)
namespace stdx { using namespace binder; }
#endif

#endif
