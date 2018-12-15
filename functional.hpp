#ifndef STDX_header_FUNCTIONAL
#define STDX_header_FUNCTIONAL

#include <functional>

namespace stdx::functional
{
	template <class ValType>
	auto forward(std::remove_reference_t<ValType> & val) // Forwarding functions used with std::bind to pass stored arguments to the stored function with the correct semantics
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
			return std::bind(std::move<ValType &>, std::move(val)); // Ideally, this would pass a std::ref(val) to the inner bind, but we can't be sure that whatever value reached here is REALLY an lvalue
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

	// TODO: investigate possible improvements with lambda

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
}

#endif

//=====

#if defined(STDX_USING_FUNCTIONAL) || defined(STDX_USING_ALL)

namespace stdx 
{ 
	using namespace functional;
}

#endif
