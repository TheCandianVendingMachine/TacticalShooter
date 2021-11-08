// window.hpp
// Wrapper around GLFW window. Initialises OpenGL context and any features wanted
#pragma once
#include <glm/vec3.hpp>
#include "messaging/eventSource.hpp"

struct GLFWwindow;
class vertexArray;
class window : public eventSource
    {
        private:
            GLFWwindow *m_window = nullptr;
            int m_width = 0;
            int m_height = 0;

            bool m_cursorEnabled = true;

            friend void windowResizeCallback(GLFWwindow *app, int width, int height);
            friend void framebufferResizeCallback(GLFWwindow *app, int width, int height);
            friend void scrollCallback(GLFWwindow *app, double xOffset, double yOffset);

        public:
            const bool &cursorEnabled = m_cursorEnabled;

            const int &width = m_width;
            const int &height = m_height;

            window(int width, int height, const char *title);
            ~window();

            GLFWwindow *getWindow() const;

            void pollEvents() const;
            bool isOpen() const;

            void clear(glm::vec3 color) const;
            void draw(const vertexArray &vao) const;
            void display() const;

            void enableCursor(bool enabled);
            void close();
    };
