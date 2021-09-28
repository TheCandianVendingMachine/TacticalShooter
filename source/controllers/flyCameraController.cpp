#include "flyCameraController.hpp"
#include "inputHandler.hpp"
#include <glm/glm.hpp>

void flyCameraController::resetMouse()
	{
		m_firstMouse = true;
	}

glm::vec3 flyCameraController::getDeltaPosition(int keyForward, int keyBackward, int keyLeft, int keyRight, glm::vec3 forward, glm::vec3 up, float deltaTime)
    {
		glm::vec3 deltaPosition = { 0, 0, 0 };
		
        if (globals::g_inputs->keyState(keyForward) == inputHandler::inputState::PRESS)
			{
				deltaPosition += forward * speed * deltaTime;
			}
		if (globals::g_inputs->keyState(keyBackward) == inputHandler::inputState::PRESS)
			{
				deltaPosition -= forward * speed * deltaTime;
			}

		if (globals::g_inputs->keyState(keyLeft) == inputHandler::inputState::PRESS)
			{
				deltaPosition -= glm::cross(forward, up) * speed * deltaTime;
			}
		if (globals::g_inputs->keyState(keyRight) == inputHandler::inputState::PRESS)
			{
				deltaPosition += glm::cross(forward, up) * speed * deltaTime;
			}

		return deltaPosition;
    }

glm::vec2 flyCameraController::getDeltaPitchYaw()
	{
		glm::vec2 mousePos = globals::g_inputs->getCursorPosition();

		if (m_firstMouse)
			{
				m_lastMousePos = mousePos;
				m_firstMouse = false;
			}

		glm::vec2 mouseDelta = (mousePos - m_lastMousePos) * mouseSensitivity;
		m_lastMousePos = mousePos;

		return glm::vec2{ mouseDelta.y, mouseDelta.x };
	}
