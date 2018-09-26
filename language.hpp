#ifndef STDX_LANGUAGE_HPP
#define STDX_LANGUAGE_HPP

#define	STDX_ENQUOTE_IMPLEMENTATION(token) #token
#define	STDX_ENQUOTE(token) STDX_ENQUOTE_IMPLEMENTATION(token)

#define	STDX_CONCATENATE_IMPLEMENTATION(token1, token2)	token1 ## token2
#define	STDX_CONCATENATE(token1, token2) STDX_CONCATENATE_IMPLEMENTATION(token1, token2)

// Enables while-else loops, disables do-while loops
#define STDX_WHILE_ELSE "while_else.lpp"
// Enables do-while loops, disables while-else loops
#define STDX_DO_WHILE "do_while.lpp"

// See https://www.chiark.greenend.org.uk/~sgtatham/mp/

#endif
