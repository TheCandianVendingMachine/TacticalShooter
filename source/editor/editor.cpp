#include "editor.hpp"
#include "inputHandler.hpp"
#include "str.hpp"
#include "graphics/window.hpp"
#include "graphics/graphicsEngine.hpp"
#include "graphics/vertex.hpp"
#include "glad/glad.h"
#include <vector>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <spdlog/spdlog.h>

/*
    FEATURE: Billboard representations of lights
    FEATURE: Billboard representations of sound emitters
    FEATURE: Dragging blocks as structures
    FEATURE: Represent camera in orthographic views
    FEATURE: Entity placement
    FEATURE: Object properties and meta-data about objects/entities
    FEATURE: Saving/Loading
*/

void editor::initKeybinds()
    {
        globals::g_inputs->addDefaultKey("editor", "camera forward", GLFW_KEY_W);
        globals::g_inputs->addDefaultKey("editor", "camera backward", GLFW_KEY_S);
        globals::g_inputs->addDefaultKey("editor", "camera left", GLFW_KEY_A);
        globals::g_inputs->addDefaultKey("editor", "camera right", GLFW_KEY_D);
    }

void editor::drawLeftPanel()
    {
        ImGui::Text("Left");
    }

void editor::drawRightPanel()
    {
        ImGui::Text("Right");
    }

void editor::drawTopPanel()
    {
        ImGui::Text("Top");
    }

void editor::drawBottomPanel()
    {
        ImGui::Text("Bottom");
    }

void editor::drawViewportFramebuffer(glm::vec2 extent, unsigned int texture)
    {
        ImVec2 origin = ImGui::GetCursorScreenPos();
        ImVec2 offset = {
            origin.x + extent.x,
            origin.y + extent.y
        };

        ImDrawList *list = ImGui::GetWindowDrawList();
        list->AddImage(
            reinterpret_cast<void*>(texture),
            origin,
            offset
        );
    }

void editor::drawEditorViewports(glm::vec2 topLeft, glm::vec2 bottomRight)
    {
        const glm::vec2 extent = (bottomRight - topLeft) / 2.f;
        m_camera3d.aspectRatio = extent.x / extent.y;

        m_topCamera.left = { 0, 0 };
        m_topCamera.right = extent * m_topOrthoExtentModifier;

        m_frontCamera.left = { 0, 0 };
        m_frontCamera.right = extent * m_frontOrthoExtentModifier;
        
        m_rightCamera.left = { 0, 0 };
        m_rightCamera.right = extent * m_rightOrthoExtentModifier;

        viewports previousViewport = m_activeViewport;
        m_activeViewport = viewports::NONE;

        // 4 viewports: 3d, top, right, front
        if ((m_mode & mode::DRAW_4_EDITORS) == mode::DRAW_4_EDITORS) 
            {
                m_3dEngine.draw(m_topCamera, m_topFramebuffer, graphicsEngine::drawFlags::EDITOR_ORTHO_PIPELINE);
                m_3dEngine.draw(m_rightCamera, m_rightFramebuffer, graphicsEngine::drawFlags::EDITOR_ORTHO_PIPELINE);
                m_3dEngine.draw(m_frontCamera, m_frontFramebuffer, graphicsEngine::drawFlags::EDITOR_ORTHO_PIPELINE);

                ImGui::Begin("#editor 3d", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize({ extent.x, extent.y });
                ImGui::SetWindowPos({ topLeft.x, topLeft.y });

                if (ImGui::IsItemActive()) 
                    {
                        m_window.enableCursor(false);
                        m_activeViewport = viewports::VIEWPORT_3D;
                    }
                else if (previousViewport == viewports::VIEWPORT_3D)
                    {
                        m_window.enableCursor(true);
                        m_camera3dController.resetMouse();
                    }

                drawViewportFramebuffer(extent, m_3dFramebufferColour);
                ImGui::Text("3d View");

                ImGui::End();

                ImGui::Begin("#editor top", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize({ extent.x, extent.y });
                ImGui::SetWindowPos({ topLeft.x + extent.x, topLeft.y });

                if (ImGui::IsItemActive()) 
                    {
                        m_activeViewport = viewports::VIEWPORT_TOP;
                    }
                else if (previousViewport == viewports::VIEWPORT_TOP)
                    {
                        m_orthographicController.resetMouse();
                    }

                drawViewportFramebuffer(extent, m_topFramebufferColour);

                ImGui::Text("Top view");

                ImGui::End();

                ImGui::Begin("#editor right", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize({ extent.x, extent.y });
                ImGui::SetWindowPos({ topLeft.x, topLeft.y + extent.y });

                if (ImGui::IsItemActive()) 
                    { 
                        m_activeViewport = viewports::VIEWPORT_RIGHT;
                    }
                else if (previousViewport == viewports::VIEWPORT_RIGHT)
                    {
                        m_orthographicController.resetMouse();
                    }

                drawViewportFramebuffer(extent, m_rightFramebufferColour);

                ImGui::Text("Right view");

                ImGui::End();

                ImGui::Begin("#editor front", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize({ extent.x, extent.y });
                ImGui::SetWindowPos({ topLeft.x + extent.x, topLeft.y + extent.y });

                if (ImGui::IsItemActive()) 
                    {
                        m_activeViewport = viewports::VIEWPORT_FRONT;
                    }
                else if (previousViewport == viewports::VIEWPORT_FRONT)
                    {
                        m_orthographicController.resetMouse();
                    }

                drawViewportFramebuffer(extent, m_frontFramebufferColour);

                ImGui::Text("Front view");

                ImGui::End();
            }
        else
            {
                ImGui::Begin("#editor 3d", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
                ImGui::SetWindowSize({ extent.x * 2.f, extent.y * 2.f });
                ImGui::SetWindowPos({ topLeft.x, topLeft.y });

                if (ImGui::IsItemActive()) 
                    {
                        m_window.enableCursor(false);
                        m_activeViewport = viewports::VIEWPORT_3D;
                    }
                else
                    {
                        m_window.enableCursor(true);
                        m_camera3dController.resetMouse();
                    }

                drawViewportFramebuffer(extent * 2.f, m_3dFramebufferColour);
                ImGui::Text("3d View");

                ImGui::End();
            }
    }

void editor::generateFramebuffers(glm::vec2 extent)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_3dFramebuffer);
        glBindTexture(GL_TEXTURE_2D, m_3dFramebufferColour);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, extent.x, extent.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_3dFramebufferColour, 0);
    
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            {
                spdlog::error("[Editor] Framebuffer is not complete while generating attachments");
            }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        unsigned int frameBuffers[3] = {
            m_frontFramebuffer, m_topFramebuffer, m_rightFramebuffer
        };

        unsigned int frameBufferColours[3] = {
            m_frontFramebufferColour, m_topFramebufferColour, m_rightFramebufferColour
        };

        for (int i = 0; i < 3; i++) 
            {
                glBindFramebuffer(GL_FRAMEBUFFER, frameBuffers[i]);
                glBindTexture(GL_TEXTURE_2D, frameBufferColours[i]);

                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, extent.x, extent.y, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBufferColours[i], 0);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                    {
                        spdlog::error("[Editor (Grid)] Framebuffer is not complete while generating attachments");
                    }

                glBindFramebuffer(GL_FRAMEBUFFER, 0);
            }
    }

editor::editor(window &window, graphicsEngine &engine3d) : 
    m_window(window),
    m_3dEngine(engine3d)
    {
        initKeybinds();

        m_camera3d.setPitchYaw(0.f, 0.f);

        m_topCamera.setPitchYaw(90.f, 0.f);
        m_topCamera.up = { 0.f, 0.f, 1.f };

        m_frontCamera.setPitchYaw(0.f, 0.f);
        m_rightCamera.setPitchYaw(0.f, 90.f);

        m_topCamera.position = { 0.f, -c_cameraDistance / 2.f, 0.f };
        m_frontCamera.position = { c_cameraDistance / 2.f, 0.f, 0.f };
        m_rightCamera.position = { 0.f, 0.f, c_cameraDistance / 2.f };

        m_topCamera.zFar = c_cameraDistance;
        m_frontCamera.zFar = c_cameraDistance;
        m_rightCamera.zFar = c_cameraDistance;

        m_topCamera.zNear = -c_cameraDistance;
        m_frontCamera.zNear = -c_cameraDistance;
        m_rightCamera.zNear = -c_cameraDistance;

        glGenFramebuffers(1, &m_3dFramebuffer);
        glGenTextures(1, &m_3dFramebufferColour);

        unsigned int fbs[3];
        glGenFramebuffers(3, fbs);

        unsigned int colours[3];
        glGenTextures(3, colours);

        m_frontFramebuffer = fbs[0];
        m_rightFramebuffer = fbs[1];
        m_topFramebuffer = fbs[2];

        m_frontFramebufferColour = colours[0];
        m_rightFramebufferColour = colours[1];
        m_topFramebufferColour = colours[2];

        generateFramebuffers({ m_window.width, m_window.height });

        m_window.subscribe(FE_STR("framebufferResize"), [this](message &event) {
            generateFramebuffers({ event.arguments[0].variable.INT, event.arguments[1].variable.INT });
        });

        m_window.subscribe(FE_STR("scroll"), [this](message &event) {
            float yOffset = static_cast<float>(event.arguments[1].variable.DOUBLE);
            float dy = yOffset * m_minExtent;

            switch (m_activeViewport)
                {
                    case editor::viewports::NONE:
                        break;
                    case editor::viewports::VIEWPORT_3D:
                        break;
                    case editor::viewports::VIEWPORT_FRONT:
                        m_frontOrthoExtentModifier += -dy;
                        m_frontOrthoExtentModifier = glm::clamp(m_frontOrthoExtentModifier, m_minExtent, m_maxExtent);
                        break;
                    case editor::viewports::VIEWPORT_TOP:
                        m_topOrthoExtentModifier += -dy;
                        m_topOrthoExtentModifier = glm::clamp(m_topOrthoExtentModifier, m_minExtent, m_maxExtent);
                        break;
                    case editor::viewports::VIEWPORT_RIGHT:
                        m_rightOrthoExtentModifier += -dy;
                        m_rightOrthoExtentModifier = glm::clamp(m_rightOrthoExtentModifier, m_minExtent, m_maxExtent);
                        break;
                    default:
                        break;
                }
        });
    }

editor::~editor()
    {
        unsigned int framebuffers[4] = {
            m_3dFramebuffer,
            m_topFramebuffer,
            m_rightFramebuffer,
            m_frontFramebuffer
        };
        glDeleteFramebuffers(4, framebuffers);

        unsigned int textures[4] = {
            m_3dFramebufferColour,
            m_topFramebufferColour,
            m_rightFramebufferColour,
            m_frontFramebufferColour
        };
        glDeleteTextures(4, textures);
    }

void editor::update()
    {
    }

void editor::fixedUpdate(float deltaTime)
    {
        const int forwardKeyCode = globals::g_inputs->keyCode("editor", "camera forward");
        const int backwardKeyCode = globals::g_inputs->keyCode("editor", "camera backward");
        const int leftKeyCode = globals::g_inputs->keyCode("editor", "camera left");
        const int rightKeyCode = globals::g_inputs->keyCode("editor", "camera right");

        switch (m_activeViewport) 
            {
                case viewports::VIEWPORT_3D:
                    m_camera3d.position += m_camera3dController.getDeltaPosition(
                        forwardKeyCode, backwardKeyCode, leftKeyCode, rightKeyCode,
                        m_camera3d.direction, m_camera3d.up, deltaTime
                    );
                    glm::vec2 pitchYaw = m_camera3dController.getDeltaPitchYaw();
                    m_camera3d.setPitchYaw(m_camera3d.pitch + pitchYaw.x, m_camera3d.yaw + pitchYaw.y);
                    break;
                case viewports::VIEWPORT_FRONT:
                    m_frontCamera.position += m_orthographicController.getDeltaPosition(
                        m_frontCamera.direction, m_frontCamera.up
                    ) * m_frontOrthoExtentModifier;
                    break;
                case viewports::VIEWPORT_RIGHT:
                    m_rightCamera.position += m_orthographicController.getDeltaPosition(
                        m_rightCamera.direction, m_rightCamera.up
                    ) * m_rightOrthoExtentModifier;
                    break;
                case viewports::VIEWPORT_TOP:
                    m_topCamera.position += m_orthographicController.getDeltaPosition(
                        m_topCamera.direction, m_topCamera.up
                    ) * m_topOrthoExtentModifier;
                    break;
                default:
                    break;
            }
    }

void editor::draw()
    {
        ImGui::BeginMainMenuBar();
        const ImVec2 barSize = ImGui::GetWindowSize();

        if (ImGui::BeginMenu("File"))
            {
                ImGui::EndMenu();
            }

        if (ImGui::BeginMenu("View"))
            {
                bool editor4Viewports = (m_mode & mode::DRAW_4_EDITORS) == mode::DRAW_4_EDITORS;
                if (ImGui::Checkbox("4 Editor Viewports", &editor4Viewports))
                    {
                        m_mode ^= mode::DRAW_4_EDITORS;
                    }

                ImGui::EndMenu();
            }

        ImGui::EndMainMenuBar();

        const ImVec2 totalSize = { static_cast<float>(m_window.width), static_cast<float>(m_window.height) - barSize.y };

        ImGui::Begin("#editor left panel", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize({ totalSize.x * m_leftPanelSize.x, totalSize.y - (totalSize.y * m_topPanelSize.y) - (totalSize.y * m_bottomPanelSize.y) });
        ImGui::SetWindowPos({ 0.f, barSize.y + totalSize.y * m_topPanelSize.y });

        drawLeftPanel();

        ImGui::End();

        ImGui::Begin("#editor right panel", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize({ totalSize.x * m_rightPanelSize.x, totalSize.y - (totalSize.y * m_topPanelSize.y) - (totalSize.y * m_bottomPanelSize.y) });
        ImGui::SetWindowPos({ totalSize.x - totalSize.x * m_rightPanelSize.x, barSize.y + totalSize.y * m_topPanelSize.y });

        drawRightPanel();

        ImGui::End();

        ImGui::Begin("#editor top panel", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize({ totalSize.x * m_topPanelSize.x, totalSize.y * m_topPanelSize.y });
        ImGui::SetWindowPos({ 0, barSize.y });

        drawTopPanel();

        ImGui::End();

        ImGui::Begin("#editor bottom panel", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::SetWindowSize({ totalSize.x * m_bottomPanelSize.x, totalSize.y * m_bottomPanelSize.y });
        ImGui::SetWindowPos({ 0, barSize.y + totalSize.y - totalSize.y * m_bottomPanelSize.y });

        drawBottomPanel();

        ImGui::End();

        m_3dEngine.draw(m_camera3d, m_3dFramebuffer);
        drawEditorViewports({ totalSize.x * m_leftPanelSize.x, barSize.y + totalSize.y * m_topPanelSize.y }, { totalSize.x - totalSize.x * m_rightPanelSize.x, barSize.y + totalSize.y - totalSize.y * m_bottomPanelSize.y });
    }

editor::mode operator|(const editor::mode &lhs, const editor::mode &rhs)
    {
        return static_cast<editor::mode>(static_cast<std::underlying_type<editor::mode>::type>(lhs) | static_cast<std::underlying_type<editor::mode>::type>(rhs));
    }

editor::mode operator&(const editor::mode &lhs, const editor::mode &rhs)
    {
        return static_cast<editor::mode>(static_cast<std::underlying_type<editor::mode>::type>(lhs) & static_cast<std::underlying_type<editor::mode>::type>(rhs));
    }

editor::mode operator^(const editor::mode &lhs, const editor::mode &rhs)
    {
        return static_cast<editor::mode>(static_cast<std::underlying_type<editor::mode>::type>(lhs) ^ static_cast<std::underlying_type<editor::mode>::type>(rhs));
    }

editor::mode operator|=(editor::mode &lhs, const editor::mode &rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

editor::mode operator&=(editor::mode &lhs, const editor::mode &rhs)
    {
        lhs = lhs | rhs;
        return lhs;
    }

editor::mode operator^=(editor::mode &lhs, const editor::mode &rhs)
    {
        lhs = lhs ^ rhs;
        return lhs;
    }
