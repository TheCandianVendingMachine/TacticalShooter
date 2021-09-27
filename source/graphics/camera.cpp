#include "camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 perspectiveCamera::view() const
	{
		return glm::lookAt(position, position + direction, up);
	}

glm::mat4 perspectiveCamera::projection() const
	{
		return glm::perspective(glm::radians(fov), aspectRatio, zNear, zFar);
	}

void perspectiveCamera::setPitchYaw(float pitch, float yaw)
	{
		direction = glm::normalize(glm::vec3(
			std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
			std::sin(glm::radians(pitch)),
			std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
		));

		m_pitch = pitch;
		m_yaw = yaw;
	}

glm::mat4 orthographicCamera::view() const
	{
		return glm::lookAt(position, position + direction, up);
	}

glm::mat4 orthographicCamera::projection() const
	{
		return glm::ortho(left.x, right.x, right.y, left.y, zNear, zFar);
	}

void orthographicCamera::setPitchYaw(float pitchDegrees, float yawDegrees)
	{
		direction = glm::normalize(glm::vec3(
			std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
			std::sin(glm::radians(pitch)),
			std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
		));

		m_pitch = pitch;
		m_yaw = yaw;
	}
