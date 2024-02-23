#ifndef PE_MEMORY_H_
#define PE_MEMORY_H_
#include "PE_defines.h"

namespace pecore {
	PE_NODISCARD void* PE_CALL PE_malloc(size_t size);
	void PE_CALL PE_free(void* ptr);
}
#endif // !PE_MEMORY_H_
