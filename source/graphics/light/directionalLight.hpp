// directionalLight.hpp
// Lights the entire scene from a given direction
#pragma once
#include <glm/vec3.hpp>
#include "light.hpp"

struct directionalLight
    {
        glm::vec3 direction = { 0, -1, 0 };
        light info;
    };
