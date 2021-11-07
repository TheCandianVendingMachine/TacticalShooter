// physicsSystem.hpp
// Initialises and manages physics. Refers to a physics world, and updates all physics components
#pragma once
#include <plf_colony.h>
#include "physicsComponent.hpp"
#include "physics/physicsWorld.hpp"

class physicsSystem
    {
        private:
            plf::colony<physicsComponent> m_components;
            physicsWorld &m_world;

        public:
            struct rigidBodyData
                {
                    physicsWorld::rigidType simulationType;
                    rigidBody::types rigidType = rigidBody::types::UNKNOWN;
                    physx::PxTransform transform = physx::PxTransform(0, 0, 0);
                };

            physicsSystem(physicsWorld &world);

            physicsComponent &createComponent(rigidBodyData data);

    };
