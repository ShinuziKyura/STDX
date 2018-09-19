#define STDX_MACRO_CONCAT_(token1, token2)	token1 ## token2
#define STDX_MACRO_CONCAT(token1, token2)	STDX_MACRO_CONCAT_(token1, token2)

// Enables while-else loops, disables do-while loops
#define STDX_WHILE_ELSE "while_else.lang"
// Enables do-while loops, disables while-else loops
#define STDX_DO_WHILE "do_while.lang"

// See https://www.chiark.greenend.org.uk/~sgtatham/mp/
