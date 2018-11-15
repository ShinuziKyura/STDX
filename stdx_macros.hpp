#ifndef STDX_MACROS_HPP
#define STDX_MACROS_HPP

// For internal usage only

#define	STDX_ENQUOTE_(token) #token
#define	STDX_ENQUOTE(token) STDX_ENQUOTE_(token)

#define	STDX_CONCATENATE_2_(token1, token2)	token1 ## token2
#define	STDX_CONCATENATE_2(token1, token2) STDX_CONCATENATE_2_(token1, token2)
#define	STDX_CONCATENATE_3_(token1, token2, token3) token1 ## token2 ## token3
#define	STDX_CONCATENATE_3(token1, token2, token3) STDX_CONCATENATE_3_(token1, token2, token3)
#define	STDX_CONCATENATE_4_(token1, token2, token3, token4) token1 ## token2 ## token3 ## token4
#define	STDX_CONCATENATE_4(token1, token2, token3, token4) STDX_CONCATENATE_4_(token1, token2, token3, token4)
#define	STDX_CONCATENATE_5_(token1, token2, token3, token4, token5) token1 ## token2 ## token3 ## token4 ## token5
#define	STDX_CONCATENATE_5(token1, token2, token3, token4, token5) STDX_CONCATENATE_5_(token1, token2, token3, token4, token5)
#define	STDX_CONCATENATE_6_(token1, token2, token3, token4, token5, token6) token1 ## token2 ## token3 ## token4 ## token5 ## token6
#define	STDX_CONCATENATE_6(token1, token2, token3, token4, token5, token6) STDX_CONCATENATE_6_(token1, token2, token3, token4, token5, token6)
#define	STDX_CONCATENATE_7_(token1, token2, token3, token4, token5, token6, token7) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7
#define	STDX_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7) STDX_CONCATENATE_7_(token1, token2, token3, token4, token5, token6, token7)
#define	STDX_CONCATENATE_8_(token1, token2, token3, token4, token5, token6, token7, token8) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8
#define	STDX_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8) STDX_CONCATENATE_8_(token1, token2, token3, token4, token5, token6, token7, token8)
#define	STDX_CONCATENATE_9_(token1, token2, token3, token4, token5, token6, token7, token8, token9) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8 ## token9
#define	STDX_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9) STDX_CONCATENATE_9_(token1, token2, token3, token4, token5, token6, token7, token8, token9)

#if defined(_MSVC_LANG) || defined(__GNUG__) || defined(__clang__)
#define STDX_UNIQUE __COUNTER__
#else
#error STDX_UNIQUE: Cannot guarantee uniqueness
//#define STDX_UNIQUE __LINE__
#endif

#define STDX_MACRO_TYPE(identifier, context, unique) STDX_CONCATENATE_5(identifier, _type_, context, _, unique)
#define STDX_MACRO_FUNCTION_0_ary(function) STDX_CONCATENATE_2(function, _)(function, STDX_UNIQUE)
#define STDX_MACRO_FUNCTION_n_ary(function, ...) STDX_CONCATENATE_2(function, _)(function, STDX_UNIQUE, __VA_ARGS__)
#define STDX_MACRO_VARIABLE(identifier, context, unique) STDX_CONCATENATE_5(identifier, _variable_, context, _, unique)

#endif