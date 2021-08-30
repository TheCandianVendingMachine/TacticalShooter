#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 camera::view() const
	{
		return glm::lookAt(position, position + direction, up);
	}

glm::mat4 camera::projection() const
	{
		return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
	}

void camera::setPitchYaw(float pitch, float yaw)
	{
		direction = glm::normalize(glm::vec3(
			std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
			std::sin(glm::radians(pitch)),
			std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
		));

		m_pitch = pitch;
		m_yaw = yaw;
	}

