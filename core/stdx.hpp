#ifndef STDX_header_CORE
#define STDX_header_CORE

// TODO rebuild file structure and project include search directories

#define STDX_implementation_ENQUOTE(token) #token
#define	STDX_ENQUOTE(token) STDX_implementation_ENQUOTE(token)

#define STDX_implementation_CONCATENATE_2(token1, token2) token1 ## token2
#define	STDX_CONCATENATE_2(token1, token2) STDX_implementation_CONCATENATE_2(token1, token2)
#define STDX_implementation_CONCATENATE_3(token1, token2, token3) token1 ## token2 ## token3
#define	STDX_CONCATENATE_3(token1, token2, token3) STDX_implementation_CONCATENATE_3(token1, token2, token3)
#define STDX_implementation_CONCATENATE_4(token1, token2, token3, token4) token1 ## token2 ## token3 ## token4
#define	STDX_CONCATENATE_4(token1, token2, token3, token4) STDX_implementation_CONCATENATE_4(token1, token2, token3, token4)
#define STDX_implementation_CONCATENATE_5(token1, token2, token3, token4, token5) token1 ## token2 ## token3 ## token4 ## token5
#define STDX_CONCATENATE_5(token1, token2, token3, token4, token5) STDX_implementation_CONCATENATE_5(token1, token2, token3, token4, token5)
#define STDX_implementation_CONCATENATE_6(token1, token2, token3, token4, token5, token6) token1 ## token2 ## token3 ## token4 ## token5 ## token6
#define	STDX_CONCATENATE_6(token1, token2, token3, token4, token5, token6) STDX_implementation_CONCATENATE_6(token1, token2, token3, token4, token5, token6)
#define STDX_implementation_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7
#define	STDX_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7) STDX_implementation_CONCATENATE_7(token1, token2, token3, token4, token5, token6, token7)
#define STDX_implementation_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8
#define	STDX_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8) STDX_implementation_CONCATENATE_8(token1, token2, token3, token4, token5, token6, token7, token8)
#define STDX_implementation_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9) token1 ## token2 ## token3 ## token4 ## token5 ## token6 ## token7 ## token8 ## token9
#define	STDX_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9) STDX_implementation_CONCATENATE_9(token1, token2, token3, token4, token5, token6, token7, token8, token9)

// TODO find better way to make directives
// #define STDX_DIRECTIVE(file, directive) STDX_ENQUOTE(STDX_CONCATENATE_4(./core/directives/, file, /, directive))

#if defined(__clang__) || defined(__GNUG__) || defined(_MSC_VER)
	#define STDX_UNIQUE() __COUNTER__
#elif 1
	#define STDX_UNIQUE() __LINE__
#else
	#error STDX_UNIQUE(): Cannot guarantee uniqueness
#endif

// For internal usage only

#define STDX_MACRO_FUNCTION_0_ARY(identifier) STDX_CONCATENATE_2(STDX_implementation_, identifier)(STDX_CONCATENATE_3(identifier, _, STDX_UNIQUE()))
#define STDX_MACRO_FUNCTION_N_ARY(identifier, ...) STDX_CONCATENATE_2(STDX_implementation_, identifier)(STDX_CONCATENATE_3(identifier, _, STDX_UNIQUE()), __VA_ARGS__)
#define STDX_MACRO_TYPE(identifier, context) STDX_CONCATENATE_4(STDX_implementation_, context, _TYPE_, identifier)
#define STDX_MACRO_VARIABLE(identifier, context) STDX_CONCATENATE_4(STDX_implementation_, context, _VARIABLE_, identifier)

#endif
