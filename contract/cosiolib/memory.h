#pragma once

#include <string.h>

extern "C" {
	
void* sbrk(size_t num_bytes);
void* malloc(size_t size);
void* calloc(size_t count, size_t size);
void* realloc(void* ptr, size_t size);
void free(void* ptr);

}
