// rigidBody.hpp
// Wrapper around PhysX rigid bodies
#pragma once

namespace physx
    {
        class PxMaterial;
        class PxRigidActor;
    }

struct rigidBody
    {
        physx::PxMaterial *material = nullptr;
        physx::PxRigidActor *actor = nullptr;

        enum class types : char {
            UNKNOWN,
            PLANE,
            SPHERE
        } type = types::UNKNOWN;
    };
