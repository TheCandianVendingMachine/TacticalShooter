// light.hpp
// information regarding the light itself: colour, intensity, etc
#pragma once
#include <glm/vec3.hpp>

struct light
    {
        glm::vec3 ambient = { 1.f, 1.f, 1.f };
        glm::vec3 diffuse = { 1.f, 1.f, 1.f };

        float radius() const
            {
                constexpr float epsilon = 0.001f;

                // c * 1/d^2 <= e
                // 1/d^2 <= e/c
                // d^2 <= c/e
                // d <= sqrt(c/e)

                float distance = glm::length(glm::sqrt(diffuse / epsilon));

                return distance;
            }
    };
