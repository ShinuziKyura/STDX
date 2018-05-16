#ifndef MACROS_H
#define MACROS_H

#define STDX_C_CODE_BEGIN		if(1){
#define STDX_C_CODE_END			}else(void)0

#define STDX_CPP_CODE_BEGIN		[&]()->void{
#define STDX_CPP_CODE_END		}()
#define STDX_CPP_CODE_RETURN	return;

// Allows deallocation of void * (size == 0) or void ** (size > 0)
#define STDX_DEALLOC(ptr, size) \
STDX_C_CODE_BEGIN\
	size_t STDX_DEALLOC_VAR1 = 0;\
	while(STDX_DEALLOC_VAR1 <= size)\
	{\
		free(STDX_DEALLOC_VAR1++ != size ? (void*) (long) ptr[STDX_DEALLOC_VAR1 - 1] : NULL);\
	}\
	free(ptr);\
STDX_C_CODE_END

#endif//MACROS_H
