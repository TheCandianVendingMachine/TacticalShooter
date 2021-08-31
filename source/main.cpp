/*
	ENGINE: OpenGL Renderer
	ENGINE: Shadows
	ENGINE: Abstracted framebuffers
	ENGINE: Deferred rendering
	ENGINE: Font rendering
	ENGINE: PBR
	ENGINE: Multiplayer
	ENGINE: PhysX
	ENGINE: Game state machine
	ENGINE: FMOD integration
	ENGINE: Steam Audio
	ENGINE: ImGui integration
	ENGINE: Generic asset referal (hash of file in header as UID?)
	ENGINE: ECS
	ENGINE: Front end UI
	ENGINE: Resource handler
	ENGINE: Observer event handler
	ENGINE: Config handler: different configs for different files - globally accessable
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

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

int main()
	{
		fe::randomImpl c_generator;
		c_generator.startUp();
		c_generator.seed(1337);

		window app(800, 600, "Tactical Shooter Prototype");
		glfwSetInputMode(app.getWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		glfwSetInputMode(app.getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		// generate all primitives onto stack. Needs to be called after OpenGL initialisation
		primitive::plane c_plane;
		primitive::sphere c_sphere;

		graphicsEngine graphicsEngine(app);

		texture diffuse("face.png", true);
		texture specular("face_specular.png", true);

		renderObject sphere;
		sphere.vao = primitive::sphere::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TEXTURE);
		sphere.diffuse = diffuse;
		sphere.specular = specular;

		renderObject plane;
		plane.vao = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TEXTURE);
		plane.diffuse = diffuse;
		plane.specular = specular;

		sphere.transform.position = { 5.f, 0.f, 0.f };

		graphicsEngine.render(plane);
		graphicsEngine.render(sphere);

		camera cam;
		cam.position = { -5.f, 2.f, 0.f };

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

				while (accumulator >= simulationRate)
					{
						accumulator -= simulationRate;

						if (glfwGetKey(app.getWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS)
							{
								glfwSetWindowShouldClose(app.getWindow(), true);
							}

						constexpr float speed = 10.f;
						if (glfwGetKey(app.getWindow(), GLFW_KEY_W) == GLFW_PRESS)
							{
								cam.position += cam.direction * speed * (float)simulationRate.asSeconds();
							}
						if (glfwGetKey(app.getWindow(), GLFW_KEY_S) == GLFW_PRESS)
							{
								cam.position -= cam.direction * speed * (float)simulationRate.asSeconds();
							}

						if (glfwGetKey(app.getWindow(), GLFW_KEY_A) == GLFW_PRESS)
							{
								cam.position -= glm::cross(cam.direction, cam.up) * speed * (float)simulationRate.asSeconds();
							}
						if (glfwGetKey(app.getWindow(), GLFW_KEY_D) == GLFW_PRESS)
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

				graphicsEngine.draw(cam);
				app.display();
				app.pollEvents();
			}

		return 0;
	}