#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 camera::view() const
    {
        return glm::lookAt(position, position + direction, up);
    }

void camera::setPitchYaw(float pitchDegrees, float yawDegrees)
    {
        m_pitch = pitchDegrees;
        m_yaw = yawDegrees;

        direction = glm::normalize(glm::vec3(
            std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
            std::sin(glm::radians(pitch)),
            std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
        ));
    }

glm::mat4 perspectiveCamera::projection() const
    {
        return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
    }

glm::mat4 orthographicCamera::projection() const
    {
        return glm::ortho(left.x, right.x, right.y, left.y, zNear, zFar);
    }

