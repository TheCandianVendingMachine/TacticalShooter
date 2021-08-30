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
	ENGINE: Generic asset referal (hash of file in header as UID?)
	ENGINE: ECS
	ENGINE: Front end UI
	ENGINE: Resource handler
	ENGINE: Observer event handler
	ENGINE: Config handler: different configs for different files - globally accessable
	FEATURE: 3d level editor

	TODO: 3d rendering
		- perspective matrices
		- abstracted
*/
#include "graphics/window.hpp"
#include "graphics/primitives.hpp"
#include "graphics/shader.hpp"
#include "graphics/camera.hpp"

#include "random.hpp"
#include "clock.hpp"
#include "time.hpp"

#include <spdlog/spdlog.h>

int main()
	{
		fe::randomImpl c_generator;
		c_generator.startUp();
		c_generator.seed(1337);

		window app(800, 600, "Tactical Shooter Prototype");

		// generate all primitives onto stack. Needs to be called after OpenGL initialisation
		primitive::plane c_plane;

		vertexArray vao = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::COLOUR);

		shader testShader("quad.vs", "quad.fs");

		fe::clock runClock;
		constexpr fe::time simulationRate = fe::seconds(1.f / 60.f);
		fe::time lastTime = runClock.getTime();
		fe::time accumulator = 0.0;

		while (app.isOpen())
			{
				fe::time currentTime = runClock.getTime();
				fe::time deltaTime = currentTime - lastTime;
				if (deltaTime >= fe::seconds(3))
					{
						deltaTime = fe::seconds(3);
					}
				lastTime = currentTime;
				accumulator += deltaTime;

				app.pollEvents();

				while (accumulator >= simulationRate)
					{
						accumulator -= simulationRate;
					}

				app.clear({ 0.1f, 0.1f, 0.1f });

				testShader.use();

				app.draw(vao);

				app.display();
			}

		return 0;
	}