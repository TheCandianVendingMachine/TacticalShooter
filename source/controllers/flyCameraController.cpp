#include "flyCameraController.hpp"
#include "inputHandler.hpp"

glm::vec3 flyCameraController::getDeltaPosition(float deltaTime)
    {
		glm::vec3 deltaPosition;
		
        if (globals::g_inputs->keyState("foo", "forward") == inputHandler::inputState::PRESS)
			{
				deltaPosition += cam.direction * speed * deltaTime;
			}
		if (globals::g_inputs->keyState("foo", "backward") == inputHandler::inputState::PRESS)
			{
				deltaPosition -= cam.direction * speed * deltaTime;
			}

		if (globals::g_inputs->keyState("foo", "left") == inputHandler::inputState::PRESS)
			{
				deltaPosition -= glm::cross(cam.direction, cam.up) * speed * deltaTime;
			}
		if (globals::g_inputs->keyState("foo", "right") == inputHandler::inputState::PRESS)
			{
				deltaPosition += glm::cross(cam.direction, cam.up) * speed * deltaTime;
			}

		double currentMouseX, currentMouseY;
		glfwGetCursorPos(app.getWindow(), &currentMouseX, &currentMouseY);

		if (m_firstMouse)
			{
				m_lastMousePosX = currentMouseX;
				m_lastMousePosY = currentMouseY;
				m_firstMouse = false;
			}

		double xDelta = (currentMouseX - m_lastMousePosX) * mouseSensitivity;
		double yDelta = (currentMouseY - m_lastMousePosY) * mouseSensitivity;

		m_lastMousePosX = currentMouseX;
		m_lastMousePosY = currentMouseY;

		float currentYaw = cam.yaw;
		float currentPitch = cam.pitch;

		currentYaw += xDelta;
		currentPitch -= yDelta;

		cam.setPitchYaw(currentPitch, currentYaw);
    }
