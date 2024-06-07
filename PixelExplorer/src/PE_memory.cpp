// Copyright (c) 2024 Austin Harms
// License can be found in LICENSE.md at the root of the repository or at the following address: https://gist.githubusercontent.com/austinharms/9bec1c9e93e12594748a41c60dd63a8d/raw/LICENSE.md

#include "PE_memory.h"
#include <malloc.h>

void* (PE_malloc)(size_t size, const char8_t* file, const char8_t* function, int line)
{
	return malloc(size);
}

void* (PE_malloc_aligned)(size_t size, size_t alignment, const char8_t* file, const char8_t* function, int line)
{
	return _aligned_malloc(size, alignment);
}

void (PE_free)(void* ptr)
{
	free(ptr);
}
