// flyCameraController.hpp
// Controller to allow flying around in 3d space
#pragma once
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class flyCameraController
    {
        private:
            bool m_firstMouse = true;
            glm::vec2 m_lastMousePos = { 0, 0 };

        public:
            float speed = 10.f;
            float mouseSensitivity = 0.1f;

            void resetMouse();

            glm::vec3 getDeltaPosition(int keyForward, int keyBackward, int keyLeft, int keyRight, glm::vec3 forward, glm::vec3 up, float deltaTime);
            glm::vec2 getDeltaPitchYaw();
    };
