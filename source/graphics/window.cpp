#include "window.hpp"
#include "vertexArray.hpp"
#include "str.hpp"
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>
#include <exception>

void openGLDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
	{
		constexpr const char *sourceStrings[] = {
			"UNDEFINED", "API", "WINDOW", "SHADER COMPILATION", "THIRD PARTY", "APPLICATION", "OTHER"
		};

		constexpr const char *typeStrings[] = {
			"UNKNOWN", "ERROR", "DEPRECATED BEHAVIOUR", "UNDEFINED BEHAVIOUR", "PORTABILITY", "PERFORMANCE", "MARKER", "PUSH GROUP", "POP GROUP", "OTHER"
		};

		unsigned int sourceIndex = 0;
		unsigned int typeIndex = 0;
		switch (source)
			{
				case GL_DEBUG_SOURCE_API:
					sourceIndex = 1;
					break;
				case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
					sourceIndex = 2;
					break;
				case GL_DEBUG_SOURCE_SHADER_COMPILER:
					sourceIndex = 3;
					break;
				case GL_DEBUG_SOURCE_THIRD_PARTY:
					sourceIndex = 4;
					break;
				case GL_DEBUG_SOURCE_APPLICATION:
					sourceIndex = 5;
					break;
				case GL_DEBUG_SOURCE_OTHER:
					sourceIndex = 6;
					break;
				default:
					break;
			}

		switch (type)
			{
				case GL_DEBUG_TYPE_ERROR:
					typeIndex = 1;
					break;
				case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
					typeIndex = 2;
					break;
				case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
					typeIndex = 3;
					break;
				case GL_DEBUG_TYPE_PORTABILITY:
					typeIndex = 4;
					break;
				case GL_DEBUG_TYPE_PERFORMANCE:
					typeIndex = 5;
					break;
				case GL_DEBUG_TYPE_MARKER:
					typeIndex = 6;
					break;
				case GL_DEBUG_TYPE_PUSH_GROUP:
					typeIndex = 7;
					break;
				case GL_DEBUG_TYPE_POP_GROUP:
					typeIndex = 8;
					break;
				case GL_DEBUG_TYPE_OTHER:
					typeIndex = 9;
					break;
				default:
					break;
			}

		switch (severity)
			{
				case GL_DEBUG_SEVERITY_HIGH:
					spdlog::error("OpenGL Error [Source: {} | Type: {}]: {}", sourceStrings[sourceIndex], typeStrings[typeIndex], message);
					break;
				case GL_DEBUG_SEVERITY_MEDIUM:
					spdlog::error("OpenGL Error [Source: {} | Type: {}]: {}", sourceStrings[sourceIndex], typeStrings[typeIndex], message);
					break;
				case GL_DEBUG_SEVERITY_LOW:
					spdlog::warn("OpenGL Warning [Source: {} | Type: {}]: {}", sourceStrings[sourceIndex], typeStrings[typeIndex], message);
					break;
				case GL_DEBUG_SEVERITY_NOTIFICATION:
					spdlog::info("OpenGL Information [Source: {} | Type: {}]: {}", sourceStrings[sourceIndex], typeStrings[typeIndex], message);
					break;
				default:
					break;
			}
	}

void windowResizeCallback(GLFWwindow *app, int width, int height)
	{
		window &userWindow = *static_cast<window*>(glfwGetWindowUserPointer(app));

		int oldWidth = userWindow.m_width;
		int oldHeight = userWindow.m_height;

		userWindow.m_width = width;
		userWindow.m_height = height;

		userWindow.signal(message(FE_STR("windowResize"), width, height, oldWidth, oldHeight));
	}

void framebufferResizeCallback(GLFWwindow *app, int width, int height)
	{
		window &userWindow = *static_cast<window*>(glfwGetWindowUserPointer(app));
		glViewport(0, 0, width, height);

		userWindow.signal(message(FE_STR("framebufferResize"), width, height));
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

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(&openGLDebugCallback, nullptr);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		glfwSetWindowUserPointer(m_window, this);

		glfwSetWindowSizeCallback(m_window, &windowResizeCallback);
		glfwSetFramebufferSizeCallback(m_window, &framebufferResizeCallback);
	}

window::~window()
	{
		glfwDestroyWindow(m_window);
		glfwTerminate();
	}

GLFWwindow *window::getWindow() const
	{
		return m_window;
	}

void window::pollEvents() const
	{
		glfwPollEvents();
	}

bool window::isOpen() const
	{
		return !glfwWindowShouldClose(m_window);
	}

void window::clear(glm::vec3 color) const
	{
		glClearColor(color.r, color.g, color.b, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

void window::draw(const vertexArray &vao) const
	{
		glBindVertexArray(vao.vao);
		glDrawElements(GL_TRIANGLES, vao.indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}

void window::display() const
	{
		glfwSwapBuffers(m_window);
	}

