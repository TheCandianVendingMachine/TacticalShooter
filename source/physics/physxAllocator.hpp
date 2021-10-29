// physxAllocator.hpp
#pragma once
#include <PxPhysicsAPI.h>

class physxAllocator : public physx::PxAllocatorCallback
    {
        public:
            virtual void *allocate(size_t size, const char *typeName, const char *filename, int line) override final;
            virtual void deallocate(void *ptr) override final;
    };
