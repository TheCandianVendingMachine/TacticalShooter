#include "serverGameWorld.hpp"

serverGameWorld::serverGameWorld() :
    m_physicsSystem(m_physics)
    {
    }

void serverGameWorld::update()
    {
    }

void serverGameWorld::fixedUpdate(double deltaTime)
    {
        m_physics.fixedUpdate(deltaTime);
    }
