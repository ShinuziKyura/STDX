#ifndef MACROS_H
#define MACROS_H

#ifdef STDX_SURPRESS_WARNINGS
#pragma warning(disable: 4100)
#endif

#define STDX_CODE_BEG		if(1){
#define STDX_CODE_END		}else(void)0

// Allows deallocation of void * (size == 0) or void ** (size > 0)
#define STDX_DEALLOC(ptr, size) \
STDX_CODE_BEG\
	size_t STDX_DEALLOC_VAR1 = 0;\
	while(STDX_DEALLOC_VAR1 <= size)\
	{\
		free(STDX_DEALLOC_VAR1++ != size ? (void*) (long) ptr[STDX_DEALLOC_VAR1 - 1] : NULL);\
	}\
	free(ptr);\
STDX_CODE_END

#endif//MACROS_H
