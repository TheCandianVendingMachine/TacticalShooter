// graphicsSystem.hpp
// foobar
#pragma once
#include <plf_colony.h>
#include "graphicsComponent.hpp"
#include "graphics/material.hpp"
#include "graphics/vertexArray.hpp"

class graphicsEngine;
class graphicsSystem
    {
        private:
            plf::colony<graphicsComponent> m_components;

        public:
            struct graphicData
                {
                    vertexArray vao;
                    material material;
                };

            graphicsSystem();

            graphicsComponent &createComponent(graphicData data);

    };
