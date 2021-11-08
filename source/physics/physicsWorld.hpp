// physicsWorld.hpp
// Mostly a wrapper around PhysX
#pragma once
#include <plf_colony.h>
#include <PxPhysicsAPI.h>
#include "physxAllocator.hpp"
#include "physxCPUDispatcher.hpp"
#include "physxErrorCallback.hpp"
#include "rigidBody.hpp"

class physicsWorld
    {
        private:
            plf::colony<rigidBody> m_rigidBodies;

            physxErrorCallback m_errorCallback;
            physxAllocator m_allocator;
            physxCPUDispatcher m_dispatcher;

            const physx::PxTolerancesScale c_tolerances;

            static constexpr double c_gravity = 9.81;
            static constexpr double c_length = 1.0;
            static constexpr unsigned int c_expectedRigidBodies = 512;

            static constexpr double c_defaultStaticFriction = 0.5;
            static constexpr double c_defaultDynamicFriction = 0.5;
            static constexpr double c_defaultRestitution = 1;

            physx::PxFoundation *m_foundation = nullptr;
            physx::PxPhysics *m_physics = nullptr;
            physx::PxScene *m_scene = nullptr;

        public:
            enum class rigidType 
                {
                    STATIC,
                    DYNAMIC
                };

            physicsWorld();
            ~physicsWorld();

            void fixedUpdate(double deltaTime);

            rigidBody &createBody(rigidType simulationType, rigidBody::types shapeType, physx::PxTransform transform = physx::PxTransform(0, 0, 0));
            void addToScene(rigidBody &body);
    };
