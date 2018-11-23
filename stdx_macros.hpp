#ifndef STDX_MACROS_HPP
#define STDX_MACROS_HPP

// For internal usage only

#define	STDX_ENQUOTE_implementation_(token) #token
#define	STDX_ENQUOTE(token) STDX_ENQUOTE_implementation_(token)

#define	STDX_CONCATENATE_2_implementation_(token1, token2)	token1 ## token2
#define	STDX_CONCATENATE_2(token1, token2) STDX_CONCATENATE_2_implementation_(token1, token2)
#define	STDX_CONCATENATE_3_implementation_(token1, token2, token3) token1 ## token2 ## token3
#define	STDX_CONCATENATE_3(token1, token2, token3) STDX_CONCATENATE_3_implementation_(token1, token2, token3)
#define	STDX_CONCATENATE_4_implementation_(token1, token2, token3, token4) token1 ## token2 ## token3 ## token4
#define	STDX_CONCATENATE_4(token1, token2, token3, token4) STDX_CONCATENATE_4_implementation_(token1, token2, token3, token4)
#define	STDX_CONCATENATE_5_implementation_(token1, token2, token3, token4, token5) token1 ## token2 ## token3 ## token4 ## token5
#define	STDX_CONCATENATE_5(token1, token2, token3, token4, token5) STDX_CONCATENATE_5_implementation_(token1, token2, token3, token4, token5)
#define	STDX_CONCATENATE_6_implementation_(token1, token2, token3, token4, token5, token6) token1 ## token2 ## token3 ## token4 ## token5 ## token6
#define	STDX_CONCATENATE_6(token1, token2, token3, token4, token5, token6) STDX_CONCATENATE_6_implementation_(token1, token2, token3, token4, token5, token6)
#define	STDX_CONCATENATE_7_implementation_(token1, token2, token3, token4, token5, token6, token7) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7
#define	STDX_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7) STDX_CONCATENATE_7_implementation_(token1, token2, token3, token4, token5, token6, token7)
#define	STDX_CONCATENATE_8_implementation_(token1, token2, token3, token4, token5, token6, token7, token8) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8
#define	STDX_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8) STDX_CONCATENATE_8_implementation_(token1, token2, token3, token4, token5, token6, token7, token8)
#define	STDX_CONCATENATE_9_implementation_(token1, token2, token3, token4, token5, token6, token7, token8, token9) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8 ## token9
#define	STDX_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9) STDX_CONCATENATE_9_implementation_(token1, token2, token3, token4, token5, token6, token7, token8, token9)

#if defined(__clang__) || defined(__GNUG__) || defined(_MSC_VER)
#define STDX_UNIQUE __COUNTER__
#else
#error STDX_UNIQUE: Cannot guarantee uniqueness
//#define STDX_UNIQUE __LINE__
#endif

#define STDX_MACRO_TYPE(identifier, context) STDX_CONCATENATE_4(identifier, _, context, _type_)
#define STDX_MACRO_FUNCTION_0_ary(identifier) STDX_CONCATENATE_2(identifier, _function_)(STDX_CONCATENATE_3(identifier, _, STDX_UNIQUE))
#define STDX_MACRO_FUNCTION_n_ary(identifier, ...) STDX_CONCATENATE_2(identifier, _function_)(STDX_CONCATENATE_3(identifier, _, STDX_UNIQUE), __VA_ARGS__)
#define STDX_MACRO_VARIABLE(identifier, context) STDX_CONCATENATE_4(identifier, _, context, _variable_)

#endif