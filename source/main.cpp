/*
	ENGINE: OpenGL Renderer
	ENGINE: Shadows
	ENGINE: Abstracted framebuffers
	ENGINE: Font rendering
	ENGINE: IBL
	ENGINE: Paralax Mapping
	ENGINE: Bloom
	ENGINE: Billboarded Sprites
	ENGINE: Decals
	ENGINE: Mesh bone animations
	ENGINE: Multiplayer
	ENGINE: PhysX
	ENGINE: Game state machine
	ENGINE: FMOD integration
	ENGINE: Steam Audio
	ENGINE: Generic asset referal (hash of file in header as UID?)
	ENGINE: ECS
	ENGINE: Front end UI
	ENGINE: Resource handler
	ENGINE: Observer event handler
	FEATURE: 3d block mesh of arbitrary size
	FEATURE: 3d level editor
*/
#include "graphics/window.hpp"
#include "graphics/primitives.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"
#include "graphics/texture.hpp"
#include "graphics/light/directionalLight.hpp"
#include "graphics/light/pointLight.hpp"
#include "graphics/light/spotLight.hpp"
#include "graphics/transformable.hpp"
#include "graphics/graphicsEngine.hpp"
#include "graphics/renderObject.hpp"

#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"
#include "str.hpp"
#include "inputHandler.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "graphics/mesh.hpp"

#include "editor/editor.hpp"

int main()
	{
		spdlog::set_level(spdlog::level::debug);

		fe::randomImpl c_generator;
		c_generator.startUp();
		c_generator.seed(1337);

		window app(1280, 720, "Tactical Shooter Prototype");
		glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		// generate all primitives onto stack. Needs to be called after OpenGL initialisation
		primitive::plane c_plane;
		primitive::sphere c_sphere;

		graphicsEngine graphicsEngine(app);

		inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
		globals::g_inputs = &c_inputHandler;

		c_inputHandler.addDefaultKey("movement", "forward", GLFW_KEY_W);
		c_inputHandler.addDefaultKey("movement", "backward", GLFW_KEY_S);
		c_inputHandler.addDefaultKey("movement", "left", GLFW_KEY_A);
		c_inputHandler.addDefaultKey("movement", "right", GLFW_KEY_D);

		c_inputHandler.addDefaultKey("debug", "close", GLFW_KEY_ESCAPE);
		c_inputHandler.addDefaultKey("debug", "toggleCursor", GLFW_KEY_F1);

		editor editor(app, graphicsEngine);

		c_inputHandler.save("inputs.ini");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForOpenGL(app.getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		constexpr fe::time simulationRate = fe::seconds(1.f / 60.f);
		constexpr fe::time maxDeltaTime = fe::seconds(3);
		fe::clock runClock;
		fe::time lastTime = runClock.getTime();
		fe::time accumulator;

		while (app.isOpen())
			{
				fe::time currentTime = runClock.getTime();
				fe::time deltaTime = currentTime - lastTime;
				if (deltaTime >= maxDeltaTime)
					{
						deltaTime = maxDeltaTime;
					}
				lastTime = currentTime;
				accumulator += deltaTime;

				ImGui_ImplOpenGL3_NewFrame();
				ImGui_ImplGlfw_NewFrame();
				ImGui::NewFrame();

				if (globals::g_inputs->keyState("debug", "close") == inputHandler::inputState::PRESS)
					{
						app.close();
					}

				editor.update();

				while (accumulator >= simulationRate)
					{
						accumulator -= simulationRate;

						editor.fixedUpdate(static_cast<float>(deltaTime.asSeconds()));
					}

				editor.draw();

				ImGui::Render();

				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				app.display();
				app.pollEvents();
			}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		return 0;
	}