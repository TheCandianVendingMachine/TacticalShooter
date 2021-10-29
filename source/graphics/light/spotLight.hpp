// spotLight.hpp
// Casts light within a cone
#pragma once
#include <glm/vec3.hpp>
#include "light.hpp"

struct spotLight
    {
        glm::vec3 position = { 0, 0, 0 };
        glm::vec3 direction = { 0, 0, 0 };
        float cutoffAngleCos = .707f; // roughly 45 degrees -> 90 degree cone
        float outerCutoffAngleCos = .707f; // roughly 45 degrees -> 90 degree cone
        light info;
    };
