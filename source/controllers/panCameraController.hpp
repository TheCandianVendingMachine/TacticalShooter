// panCameraController.hpp
// Allows the user to pan a 2d camera and zoom in/out
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class panCameraController
    {
        private:
            bool m_firstMouse = true;
            glm::vec2 m_lastMousePos = { 0, 0 };

        public:
            void resetMouse();
            glm::vec3 getDeltaPosition(glm::vec3 forward, glm::vec3 up);

    };
