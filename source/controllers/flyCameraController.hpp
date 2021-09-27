// flyCameraController.hpp
// Controller to allow flying around in 3d space
#pragma once
#include <glm/vec3.hpp>

class flyCameraController
    {
        private:
        public:
            float speed = 10.f;

            glm::vec3 getDeltaPosition();
    };
