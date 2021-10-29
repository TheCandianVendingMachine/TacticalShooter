#include "physxAllocator.hpp"

void *physxAllocator::allocate(size_t size, const char *typeName, const char *filename, int line)
    {
        return malloc(size);
    }

void physxAllocator::deallocate(void *ptr)
    {
        free(ptr);
    }
