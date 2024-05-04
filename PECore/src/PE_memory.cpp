#include "PE_memory.h"
#include <malloc.h>

namespace pe {
	void* PE_malloc(size_t size) {
		return malloc(size);
	}

	void* PE_alignedMalloc(size_t size, size_t align) {
		return std::aligned_alloc(align, size);
	}

	void PE_free(void* ptr) {
		free(ptr);
	}
}
