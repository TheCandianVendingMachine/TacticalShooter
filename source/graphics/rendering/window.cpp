#include "window.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <exception>

void windowResizeCallback(GLFWwindow *app, int width, int height)
	{
		window &userWindow = *static_cast<window*>(glfwGetWindowUserPointer(app));
		userWindow.m_width = width;
		userWindow.m_height = height;
	}

void framebufferResizeCallback(GLFWwindow *app, int width, int height)
	{
		window &userWindow = *static_cast<window*>(glfwGetWindowUserPointer(app));
		glViewport(0, 0, width, height);
	}

window::window(int width, int height, const char *title) :
	m_width(width),
	m_height(height)
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_window = glfwCreateWindow(m_width, m_height, "Tactical Shooter Prototype", nullptr, nullptr);
		if (m_window == nullptr)
			{
				glfwTerminate();
				spdlog::error("Could not create window");
				std::abort();
			}
		glfwMakeContextCurrent(m_window);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
			{
				glfwTerminate();
				spdlog::error("Could not load GLAD!");
				std::abort();
			}

		glEnable(GL_DEPTH_TEST);
		glfwSetWindowUserPointer(m_window, this);

		glfwSetWindowSizeCallback(m_window, &windowResizeCallback);
		glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	}

window::~window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

void window::pollEvents() const
	{
		glfwPollEvents();
	}

bool window::isOpen() const
	{
		return !glfwWindowShouldClose(m_window);
	}

