// editor.hpp
// entry point for all editor related code
#pragma once
#include <glm/vec2.hpp>
#include "graphics/camera.hpp"

class window;
class editor
	{
		private:
			enum class mode : int
				{
					NONE			= 0,
					DRAW_4_EDITORS	= 1 << 0,
				};
			friend editor::mode operator|(const editor::mode &lhs, const editor::mode &rhs);
			friend editor::mode operator&(const editor::mode &lhs, const editor::mode &rhs);
			friend editor::mode operator^(const editor::mode &lhs, const editor::mode &rhs);

			friend editor::mode operator|=(editor::mode &lhs, const editor::mode &rhs);
			friend editor::mode operator&=(editor::mode &lhs, const editor::mode &rhs);
			friend editor::mode operator^=(editor::mode &lhs, const editor::mode &rhs);

			// sizes are in percent of screen size
			glm::vec2 m_topPanelSize		= { 1.f, 1.f / 8.f };
			glm::vec2 m_bottomPanelSize		= { 1.f, 1.f / 8.f };
			glm::vec2 m_leftPanelSize		= { 1.f / 8.f, 1.f };
			glm::vec2 m_rightPanelSize		= { 1.f / 8.f, 1.f };

			mode m_mode = mode::NONE;

			enum class viewports 
				{
					NONE,
					VIEWPORT_3D,
					VIEWPORT_FRONT,
					VIEWPORT_TOP,
					VIEWPORT_RIGHT
				} m_activeViewport;

			camera m_camera3d;

			window &m_window;

			void initKeybinds();

			void drawLeftPanel();
			void drawRightPanel();
			void drawTopPanel();
			void drawBottomPanel();

			void drawEditorViewports(glm::vec2 topLeft, glm::vec2 bottomRight);

		public:
			bool enabled = true;

			editor(window &window);

			void update();
			void fixedUpdate(float deltaTime);
			void draw();
	};
