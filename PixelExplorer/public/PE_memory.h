#ifndef PE_MEMORY_H_
#define PE_MEMORY_H_
#include "PE_defines.h"
#include "PE_stdint.h"

// Allocates {size} bytes and returns a pointer
// return nullptr on failure, pointer should be freed using PE_free
// Note: the PE_malloc macro should be used instead of this function
PE_EXTERN_C PE_API PE_NODISCARD void* PE_CALL PE_malloc(size_t size, const char8_t* file, const char8_t* function, int line);

// Allocates {size} bytes aligned to {alignment} and returns a pointer
// return nullptr on failure, pointer should be freed using PE_free
// Note: the PE_malloc_aligned macro should be used instead of this function
PE_EXTERN_C PE_API PE_NODISCARD void* PE_CALL PE_malloc_aligned(size_t size, size_t alignment, const char8_t* file, const char8_t* function, int line);

// Frees the memory pointed to by {ptr},
// {ptr} must have been allocated by PE_malloc or PE_malloc_aligned
// Note: the PE_free macro should be used instead of this function
PE_EXTERN_C PE_API void PE_CALL PE_free(void* ptr);

// Use macros to allow for easier future memory profiling/debugging

// Allocates {size} bytes and returns a pointer
// return nullptr on failure, pointer should be freed using PE_free
#define PE_malloc(size) (PE_malloc)(size, PE_FUNCTION_NAME, PE_FILE_NAME, PE_FILE_LINE)

// Allocates {size} bytes aligned to {alignment} and returns a pointer
// return nullptr on failure, pointer should be freed using PE_free
#define PE_malloc_aligned(size, alignment) (PE_malloc_aligned)(size, alignment, PE_FUNCTION_NAME, PE_FILE_NAME, PE_FILE_LINE)

// Frees the memory pointed to by {ptr}
// Note: {ptr} must have been allocated by PE_malloc or PE_malloc_aligned
#define PE_free(ptr) (PE_free)(ptr)
#endif // !PE_MEMORY_H_
