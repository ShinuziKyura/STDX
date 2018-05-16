#ifndef BINDER_HPP
#define BINDER_HPP

#include <functional>

namespace stdx
{	// Needs testing
	template <class RetType, class ... ParamTypes, class ... ArgTypes>
	auto bind(RetType(* func)(ParamTypes ...), ArgTypes && ... args)
	{
		return std::bind(func, bind_forward<ArgTypes>(args) ...);
	}
	template <class RetType, class ... ParamTypes, class ObjType, class ... ArgTypes>
	auto bind(RetType(ObjType::* func)(ParamTypes ...), ObjType * obj, ArgTypes && ... args)
	{
		return std::bind(func, obj, bind_forward<ArgTypes>(args) ...);
	}

	// Why this works: std::bind saves its arguments by copy/move constructing them into member variables of the Binder class;
	// std::move works because std::bind invokes any argument of type T for which std::is_bind_expression<T>::value == true, as described here:
	// http://en.cppreference.com/w/cpp/utility/functional/bind, 
	// requiring however the use of the std::move lvalue-reference overload, since the argument is passed by reference, as explained here:
	// https://stackoverflow.com/questions/4871273/passing-rvalues-through-stdbind

	template <class ValType>
	auto bind_forward(std::remove_reference_t<ValType> & val)
	{
		if constexpr (std::is_lvalue_reference_v<ValType>)
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
		return std::bind(std::move<ValType &>, std::move(val));
	}
}

#endif//BINDER_HPP
