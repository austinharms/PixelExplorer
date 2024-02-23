#include "PE_memory.h"

namespace pecore {
	void* PE_malloc(size_t size) {
		return malloc(size);
	}

	void PE_free(void* ptr) {
		free(ptr);
	}
}
