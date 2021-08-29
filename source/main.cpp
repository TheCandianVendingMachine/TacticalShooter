#include <GLFW/glfw3.h>

/*
	ENGINE: OpenGL Renderer
	ENGINE: Generic camera system
	ENGINE: Forward rendering
	ENGINE: Deferred rendering
	ENGINE: Abstract out OpenGL shit
	ENGINE: Font rendering
	ENGINE: PBR
	ENGINE: Multiplayer
	ENGINE: PhysX
	ENGINE: Game state machine
	ENGINE: FMOD integration
	ENGINE: Steam Audio
	ENGINE: ImGui integration
	ENGINE: Generic asset referal (unique hash per file?)
*/

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