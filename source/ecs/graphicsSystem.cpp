#include "graphicsSystem.hpp"

graphicsSystem::graphicsSystem()
    {
    }

graphicsComponent &graphicsSystem::createComponent(graphicData data)
    {
        graphicsComponent &component = *m_components.emplace();

        component.renderObject.vao = data.vao;
        component.renderObject.material = data.material;

        return component;
    }
