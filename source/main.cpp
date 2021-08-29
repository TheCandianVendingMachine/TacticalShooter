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
	FEATURE: 3d level editor
*/
#include "graphics/window.hpp"

int main()
	{
		window app(800, 600, "Tactical Shooter Prototype");

		while (app.isOpen())
			{
				app.pollEvents();
			}

		return 0;
	}