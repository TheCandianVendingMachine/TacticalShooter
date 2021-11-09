#include "panCameraController.hpp"
#include "inputHandler.hpp"
#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

void panCameraController::resetMouse()
    {
        m_firstMouse = true;
    }

glm::vec3 panCameraController::getDeltaPosition(glm::vec3 forward, glm::vec3 up)
    {
        if (m_firstMouse)
            {
                m_firstMouse = false;
                m_lastMousePos = globals::g_inputs->getCursorPosition();
            }

        glm::vec2 deltaPos = globals::g_inputs->getCursorPosition() - m_lastMousePos;
        m_lastMousePos = globals::g_inputs->getCursorPosition();

        glm::vec3 panDirection = glm::normalize(glm::cross(forward, up));
        glm::mat3 cob(panDirection, up, forward);

        glm::vec3 relativePosition = cob * glm::vec3(-deltaPos, 0);
        if (glm::any(glm::lessThan(glm::abs(relativePosition), glm::vec3(fe::EPSILON))))
            {
                if (glm::abs(relativePosition.x) < fe::EPSILON)
                    {
                        relativePosition.x = 0.f;
                    }

                if (glm::abs(relativePosition.y) < fe::EPSILON)
                    {
                        relativePosition.y = 0.f;
                    }

                if (glm::abs(relativePosition.z) < fe::EPSILON)
                    {
                        relativePosition.z = 0.f;
                    }
            }

        return relativePosition;
    }
