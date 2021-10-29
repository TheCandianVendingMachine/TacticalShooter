// physxErrorCallback.hpp
// A callback for PhysX errors
#pragma once
#include <PxPhysicsAPI.h>

class physxErrorCallback : public physx::PxErrorCallback
    {
        public:
            virtual void reportError(physx::PxErrorCode::Enum code, const char *message, const char *file, int line) override final;
    };
