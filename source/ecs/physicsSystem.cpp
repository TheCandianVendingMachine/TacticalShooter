#include "physicsSystem.hpp"

physicsSystem::physicsSystem(physicsWorld &world) :
    m_world(world)
    {
    }

physicsComponent &physicsSystem::createComponent(rigidBodyData data)
    {
        rigidBody &body = m_world.createBody(data.simulationType, data.rigidType, data.transform);
        m_world.addToScene(body);

        return *m_components.emplace(body);
    }

