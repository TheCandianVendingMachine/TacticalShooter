// serverGameWorld.hpp
// Represents the game world as on the server. The authority on everything, so its comprehensive
#pragma once
#include "physics/physicsWorld.hpp"
#include "ecs/graphicsSystem.hpp"
#include "ecs/physicsSystem.hpp"
#include "ecs/graphicsComponent.hpp"
#include "ecs/physicsComponent.hpp"
#include "ecs/entity.hpp"

class serverGameWorld
    {
        private:
            physicsWorld m_physics;

            physicsSystem m_physicsSystem;
            graphicsSystem m_graphicsSystem;

        public:
            serverGameWorld();
            void update();
            void fixedUpdate(double deltaTime);
    };
