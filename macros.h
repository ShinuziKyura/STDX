#define STDX_CODE_BEG						if(1){
#define STDX_CODE_END						}else(void)0
#define STDX_MACRO_CONCAT_(token1, token2)	token1 ## token2
#define STDX_MACRO_CONCAT(token1, token2)	STDX_MACRO_CONCAT_(token1, token2)

// Allows deallocation of void * (size == 0) or void ** (size > 0)
#define STDX_DEALLOC(ptr, size) \
STDX_CODE_BEG\
	size_t STDX_DEALLOC_VAR1 = 0;\
	while(STDX_DEALLOC_VAR1 < size)\
	{\
		free((void*) (long) ptr[STDX_DEALLOC_VAR1++]);\
	}\
	free(ptr);\
STDX_CODE_END

#define STDX_RANGE(container) ::std::begin(container), ::std::end(container)
#define STDX_RRANGE(container) ::std::rbegin(container), ::std::rend(container)

#ifdef STDX_WHILE_ELSE
// Proof of concept: implements while-else in the language; unfortunately, it breaks do-while in the process
#define while(condition) if (bool STDX_MACRO_CONCAT(STDX_WHILE_ELSE_VAR_, __LINE__) = (condition)) while ((STDX_MACRO_CONCAT(STDX_WHILE_ELSE_VAR_, __LINE__) && ((STDX_MACRO_CONCAT(STDX_WHILE_ELSE_VAR_, __LINE__) = false) == false)) || (condition))
#endif

// See https://www.chiark.greenend.org.uk/~sgtatham/mp/
