#ifndef STDX_MACROS_HPP
#define STDX_MACROS_HPP

#define STDX_CODE_BEG if(1){
#define STDX_CODE_END }else(void)0

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

#endif
