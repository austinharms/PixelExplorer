#ifndef PE_MEMORY_H_
#define PE_MEMORY_H_
#include "PE_defines.h"

namespace pe {
	PE_EXTERN_C PE_NODISCARD void* PE_CALL PE_malloc(size_t size);
	PE_EXTERN_C PE_NODISCARD void* PE_CALL PE_alignedMalloc(size_t size, size_t align);
	PE_EXTERN_C void PE_CALL PE_free(void* ptr);
}
#endif // !PE_MEMORY_H_
