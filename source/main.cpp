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
*/
#include "graphics/window.hpp"
#include "graphics/primitives.hpp"
#include "graphics/shader.hpp"

#include <spdlog/spdlog.h>

int main()
	{
		window app(800, 600, "Tactical Shooter Prototype");

		// generate all primitives onto stack. Needs to be called after OpenGL initialisation
		primitive::plane c_plane;

		vertexArray vao = primitive::plane::generate(vertex::attributes::POSITION | vertex::attributes::COLOUR);

		shader testShader("quad.vs", "quad.fs");

		while (app.isOpen())
			{
				app.pollEvents();

				app.clear({ 0.1f, 0.1f, 0.1f });

				testShader.use();

				app.draw(vao);

				app.display();
			}

		return 0;
	}