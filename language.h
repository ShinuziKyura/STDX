#define STDX_MACRO_CONCAT_(token1, token2)	token1 ## token2
#define STDX_MACRO_CONCAT(token1, token2)	STDX_MACRO_CONCAT_(token1, token2)

// If this is active, need to #include STDX_DO_WHILE before using a do-while loop
#define STDX_WHILE_ELSE "while_else.lang"
#define STDX_DO_WHILE "do_while.lang"
