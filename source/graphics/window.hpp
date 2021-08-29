// window.hpp
// Wrapper around GLFW window. Initialises OpenGL context and any features wanted
#pragma once
#include <glm/vec3.hpp>

struct GLFWwindow;
class window
	{
		private:
			GLFWwindow *m_window = nullptr;
			int m_width = 0;
			int m_height = 0;

			friend void windowResizeCallback(GLFWwindow *app, int width, int height);
			friend void framebufferResizeCallback(GLFWwindow *app, int width, int height);

		public:
			const int &width = m_width;
			const int &height = m_height;

			window(int width, int height, const char *title);
			~window();

			void pollEvents() const;
			bool isOpen() const;

			void clear(glm::vec3 color) const;
			void display() const;
	};
