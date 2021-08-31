/*
	ENGINE: OpenGL Renderer
	ENGINE: Forward rendering
	ENGINE: Pointlight
	ENGINE: Spotlight
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

#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"
#include "str.hpp"

#include <spdlog/spdlog.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

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

		vertexArray vao = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::NORMAL | vertex::attributes::TEXTURE);
		texture diffuse("face.png", true);
		texture specular("face_specular.png", true);

		directionalLight testLight;
		testLight.direction = glm::normalize(glm::vec3(0.f, 1.f, 1.f));
		testLight.info.ambient = glm::vec3(0.1f);

		testLight.info.diffuse = { 1.f, 0.f, 0.f };
		testLight.info.specular = { 0.2f, 0.2f, 0.2f };

		shader testShader("shaders/forward_lighting.vs", "shaders/forward_lighting.fs");
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

				app.clear({ 0.1f, 0.1f, 0.1f });

				testShader.use();

				glm::mat4 model(1.f);
				//model = glm::rotate(model, glm::radians(45.f), glm::vec3(0.f, 0.f, 0.f));

				glm::mat4 lightSpaceMatrix(1.f); // not used yet
				testShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

				testShader.setInt("material.diffuse", 0);
				testShader.setInt("material.specular", 1);
				testShader.setFloat("material.shininess", 128.f);

				testShader.setMat4("model", model);
				testShader.setMat4("view", cam.view());
				testShader.setMat4("projection", cam.projection());

				testShader.setVec3("ViewPos", cam.position);
				testShader.setFloat("gamma", 2.2);

				testShader.setVec3("light.direction", testLight.direction);

				testShader.setVec3("light.ambient", testLight.info.ambient);
				testShader.setVec3("light.diffuse", testLight.info.diffuse);
				testShader.setVec3("light.specular", testLight.info.specular);

				testShader.setFloat("light.constant", testLight.info.constant);
				testShader.setFloat("light.linear", testLight.info.linear);
				testShader.setFloat("light.quadratic", testLight.info.quadratic);


				diffuse.bind(GL_TEXTURE0);
				specular.bind(GL_TEXTURE1);

				app.draw(vao);

				app.display();

				app.pollEvents();
			}

		return 0;
	}