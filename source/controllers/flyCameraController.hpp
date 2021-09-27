// flyCameraController.hpp
// Controller to allow flying around in 3d space
#pragma once
#include <glm/vec3.hpp>

class flyCameraController
    {
        private:
            bool m_firstMouse = true;
            double m_lastMousePosX = 0.0;
            double m_lastMousePosY = 0.0;

        public:
            float speed = 10.f;
            float mouseSensitivity = 0.1f;

            glm::vec3 getDeltaPosition(float deltaTime);
    };
