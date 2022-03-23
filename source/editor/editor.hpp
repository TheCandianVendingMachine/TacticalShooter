// editor.hpp
// entry point for all editor related code
#pragma once
#include <glm/vec2.hpp>
#include <string>
#include "graphics/camera.hpp"
#include "graphics/vertexArray.hpp"
#include "graphics/shader.hpp"
#include "controllers/flyCameraController.hpp"
#include "controllers/panCameraController.hpp"

class window;
class graphicsEngine;
class textureManager;
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

            perspectiveCamera m_camera3d;
            flyCameraController m_camera3dController;

            orthographicCamera m_topCamera;
            orthographicCamera m_rightCamera;
            orthographicCamera m_frontCamera;
            panCameraController m_orthographicController;

            const int c_editorViewportWindowFlags = 0;

            static constexpr float m_minExtent = 1.f / 64.f;
            static constexpr float m_maxExtent = 1.f / 4.f;

            float m_topOrthoExtentModifier = 1 / 8.f;
            float m_rightOrthoExtentModifier = 1 / 8.f;
            float m_frontOrthoExtentModifier = 1 / 8.f;

            static constexpr float c_cameraDistance = 70'000.f;

            unsigned int m_3dFramebuffer = 0;
            unsigned int m_3dFramebufferColour = 0;

            unsigned int m_topFramebuffer = 0;
            unsigned int m_topFramebufferColour = 0;

            unsigned int m_rightFramebuffer = 0;
            unsigned int m_rightFramebufferColour = 0;

            unsigned int m_frontFramebuffer = 0;
            unsigned int m_frontFramebufferColour = 0;

            bool m_showingTextureManager = false;
            std::string m_selectedTexture;

            window &m_window;
            graphicsEngine &m_3dEngine;
            textureManager &m_textureManager;

            void initKeybinds();

            void drawLeftPanel();
            void drawRightPanel();
            void drawTopPanel();
            void drawBottomPanel();

            void drawViewportFramebuffer(glm::vec2 extent, unsigned int texture);
            void drawEditorViewports(glm::vec2 topLeft, glm::vec2 bottomRight);

            void drawTextureManager();

            void generateFramebuffers(glm::vec2 extent);

        public:
            bool enabled = true;

            editor(window &window, graphicsEngine &engine3d, textureManager &textureManager);
            ~editor();

            void update();
            void fixedUpdate(float deltaTime);
            void draw();
    };
