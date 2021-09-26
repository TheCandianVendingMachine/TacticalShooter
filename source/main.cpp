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
	ENGINE: 3d mesh loader
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
	ENGINE: Config handler: different configs for different files - globally accessable
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

int main()
	{
		spdlog::set_level(spdlog::level::debug);

		fe::randomImpl c_generator;
		c_generator.startUp();
		c_generator.seed(1337);

		window app(800, 600, "Tactical Shooter Prototype");
		glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		app.enableCursor(false);

		inputHandler c_inputHandler(app.getWindow(), "inputs.ini");
		c_inputHandler.addDefaultKey("movement", "forward", GLFW_KEY_W);
		c_inputHandler.addDefaultKey("movement", "backward", GLFW_KEY_S);
		c_inputHandler.addDefaultKey("movement", "left", GLFW_KEY_A);
		c_inputHandler.addDefaultKey("movement", "right", GLFW_KEY_D);

		c_inputHandler.addDefaultKey("debug", "close", GLFW_KEY_ESCAPE);
		c_inputHandler.addDefaultKey("debug", "toggleCursor", GLFW_KEY_F1);

		c_inputHandler.save("inputs.ini");

		globals::g_inputs = &c_inputHandler;

		// generate all primitives onto stack. Needs to be called after OpenGL initialisation
		primitive::plane c_plane;
		primitive::sphere c_sphere;

		graphicsEngine graphicsEngine(app);

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGui_ImplGlfw_InitForOpenGL(app.getWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 330");

		pointLight &pl = graphicsEngine.createPointLight();
		pl.position = glm::vec3(-5.f, 4.f, 0.f);
		pl.info.ambient = glm::vec3(0.1f);
		pl.info.diffuse = glm::vec3(1.4f);

		spotLight &sp = graphicsEngine.createSpotLight();
		sp.cutoffAngleCos = glm::cos(glm::radians(5.0f));
		sp.outerCutoffAngleCos = glm::cos(glm::radians(15.f));
		sp.info.ambient = glm::vec3(0.f);
		sp.info.diffuse = glm::vec3(10.f);

		camera cam;
		cam.position = { -5.f, 2.f, 0.f };
		cam.zFar = 1000.f;

		app.subscribe(FE_STR("framebufferResize"), [&cam] (message &m) {
			cam.aspectRatio = static_cast<float>(m.arguments[0].variable.INT) / static_cast<float>(m.arguments[1].variable.INT);
		});

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

				while (accumulator >= simulationRate)
					{
						accumulator -= simulationRate;

						if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("debug", "close")) == GLFW_PRESS)
							{
								glfwSetWindowShouldClose(app.getWindow(), true);
							}

						constexpr float speed = 10.f;
						if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("movement", "forward")) == GLFW_PRESS)
							{
								cam.position += cam.direction * speed * (float)simulationRate.asSeconds();
							}
						if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("movement", "backward")) == GLFW_PRESS)
							{
								cam.position -= cam.direction * speed * (float)simulationRate.asSeconds();
							}

						if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("movement", "left")) == GLFW_PRESS)
							{
								cam.position -= glm::cross(cam.direction, cam.up) * speed * (float)simulationRate.asSeconds();
							}
						if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("movement", "right")) == GLFW_PRESS)
							{
								cam.position += glm::cross(cam.direction, cam.up) * speed * (float)simulationRate.asSeconds();
							}

						constexpr double sensitivity = 0.1f;

						static bool firstMouse = true;
						static double lastMouseX = 400;
						static double lastMouseY = 300;

						double currentMouseX, currentMouseY;
						glfwGetCursorPos(app.getWindow(), &currentMouseX, &currentMouseY);

						if (firstMouse)
							{
								lastMouseX = currentMouseX;
								lastMouseY = currentMouseY;
								firstMouse = false;
							}

						double xDelta = (currentMouseX - lastMouseX) * sensitivity;
						double yDelta = (currentMouseY - lastMouseY) * sensitivity;

						lastMouseX = currentMouseX;
						lastMouseY = currentMouseY;

						float currentYaw = cam.yaw;
						float currentPitch = cam.pitch;

						currentYaw += xDelta;
						currentPitch -= yDelta;

						cam.setPitchYaw(currentPitch, currentYaw);
					}
				
#ifdef _DEBUG
				if (glfwGetKey(app.getWindow(), globals::g_inputs->keyCode("debug", "toggleCursor")) == GLFW_PRESS)
					{
						app.enableCursor(!app.cursorEnabled);
					}
#endif

				ImGui::ShowDemoWindow();

				ImGui::Render();

				graphicsEngine.draw(cam);

				ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

				app.display();
				app.pollEvents();
			}

		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		return 0;
	}