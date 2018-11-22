#ifndef STDX_MACROS_HPP
#define STDX_MACROS_HPP

// For internal usage only

#define	STDX_ENQUOTE_FUNCTION(token) #token
#define	STDX_ENQUOTE(token) STDX_ENQUOTE_FUNCTION(token)

#define	STDX_CONCATENATE_2_FUNCTION(token1, token2)	token1 ## token2
#define	STDX_CONCATENATE_2(token1, token2) STDX_CONCATENATE_2_FUNCTION(token1, token2)
#define	STDX_CONCATENATE_3_FUNCTION(token1, token2, token3) token1 ## token2 ## token3
#define	STDX_CONCATENATE_3(token1, token2, token3) STDX_CONCATENATE_3_FUNCTION(token1, token2, token3)
#define	STDX_CONCATENATE_4_FUNCTION(token1, token2, token3, token4) token1 ## token2 ## token3 ## token4
#define	STDX_CONCATENATE_4(token1, token2, token3, token4) STDX_CONCATENATE_4_FUNCTION(token1, token2, token3, token4)
#define	STDX_CONCATENATE_5_FUNCTION(token1, token2, token3, token4, token5) token1 ## token2 ## token3 ## token4 ## token5
#define	STDX_CONCATENATE_5(token1, token2, token3, token4, token5) STDX_CONCATENATE_5_FUNCTION(token1, token2, token3, token4, token5)
#define	STDX_CONCATENATE_6_FUNCTION(token1, token2, token3, token4, token5, token6) token1 ## token2 ## token3 ## token4 ## token5 ## token6
#define	STDX_CONCATENATE_6(token1, token2, token3, token4, token5, token6) STDX_CONCATENATE_6_FUNCTION(token1, token2, token3, token4, token5, token6)
#define	STDX_CONCATENATE_7_FUNCTION(token1, token2, token3, token4, token5, token6, token7) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7
#define	STDX_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7) STDX_CONCATENATE_7_FUNCTION(token1, token2, token3, token4, token5, token6, token7)
#define	STDX_CONCATENATE_8_FUNCTION(token1, token2, token3, token4, token5, token6, token7, token8) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8
#define	STDX_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8) STDX_CONCATENATE_8_FUNCTION(token1, token2, token3, token4, token5, token6, token7, token8)
#define	STDX_CONCATENATE_9_FUNCTION(token1, token2, token3, token4, token5, token6, token7, token8, token9) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8 ## token9
#define	STDX_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9) STDX_CONCATENATE_9_FUNCTION(token1, token2, token3, token4, token5, token6, token7, token8, token9)

#if defined(__clang__) || defined(__GNUG__) || defined(_MSC_VER)
#define STDX_UNIQUE __COUNTER__
#else
#error STDX_UNIQUE: Cannot guarantee uniqueness
//#define STDX_UNIQUE __LINE__
#endif

#define STDX_MACRO_TYPE(identifier, context, unique) STDX_CONCATENATE_5(identifier, _TYPE_, context, _, unique)
#define STDX_MACRO_FUNCTION_0_ary(function) STDX_CONCATENATE_2(function, _FUNCTION)(function, STDX_UNIQUE)
#define STDX_MACRO_FUNCTION_n_ary(function, ...) STDX_CONCATENATE_2(function, _FUNCTION)(function, STDX_UNIQUE, __VA_ARGS__)
#define STDX_MACRO_VARIABLE(identifier, context, unique) STDX_CONCATENATE_5(identifier, _VARIABLE_, context, _, unique)

#endif