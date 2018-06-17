#define STDX_DEFINE_INLINE_VARIABLE(type, name, value) \
template <class = void>\
struct name ## _INLINE_VARIABLE_TYPE\
{\
	name ## _INLINE_VARIABLE_TYPE() = delete;\
	static type name ## _INLINE_VARIABLE_VALUE;\
};\
template <>\
type name ## _INLINE_VARIABLE_TYPE<>:: name ## _INLINE_VARIABLE_VALUE{ value }
	
#define STDX_INLINE_VARIABLE(name) name ## _INLINE_VARIABLE_TYPE<>::name ## _INLINE_VARIABLE_VALUE
