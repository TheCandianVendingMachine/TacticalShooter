// camera.hpp
// a camera. has a position and orientation
#pragma once
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

class camera
	{
		private:
			float m_pitch = 0.f;
			float m_yaw = 0.f;

		public:
			glm::vec3 position = { 0, 0, 0 };
			glm::vec3 direction = { 0, 0, 1 };
			glm::vec3 up = { 0, 1, 0 };

			const float &pitch = m_pitch;
			const float &yaw = m_yaw;

			float fov = 45.f;
			float aspectRatio = 16.f / 9.f;
			float zNear = 0.1f;
			float zFar = 100.f;

			glm::mat4 view() const;
			glm::mat4 projection() const;

			void setPitchYaw(float pitch, float yaw);

	};
