#include <GLFW/glfw3.h>

int main()
	{
		glfwInit();

		GLFWwindow *app = glfwCreateWindow(800, 600, "Tactical Shooter Prototype", nullptr, nullptr);

		while (!glfwWindowShouldClose(app))
			{
				glfwPollEvents();
			}
		
		glfwTerminate();

		return 0;
	}