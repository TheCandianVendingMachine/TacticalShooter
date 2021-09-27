#include "editor.hpp"
#include "inputHandler.hpp"
#include "graphics/window.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>

void editor::initKeybinds()
	{
		globals::g_inputs->addDefaultKey("editor", "camera forward", GLFW_KEY_W);
		globals::g_inputs->addDefaultKey("editor", "camera backward", GLFW_KEY_S);
		globals::g_inputs->addDefaultKey("editor", "camera left", GLFW_KEY_A);
		globals::g_inputs->addDefaultKey("editor", "camera right", GLFW_KEY_D);
	}

void editor::drawLeftPanel()
	{
	}

void editor::drawRightPanel()
	{
	}

void editor::drawTopPanel()
	{
	}

void editor::drawBottomPanel()
	{
	}

void editor::drawEditorViewports(glm::vec2 topLeft, glm::vec2 bottomRight)
	{
		const glm::vec2 extent = (bottomRight - topLeft) / 2.f;

		// 4 viewports: 3d, top, right, front
		if ((m_mode & mode::DRAW_4_EDITORS) == mode::DRAW_4_EDITORS) 
			{
				m_activeViewport = viewports::NONE;

				ImGui::Begin("#editor 3d", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowSize({ extent.x, extent.y });
				ImGui::SetWindowPos({ topLeft.x, topLeft.y });

				if (ImGui::IsItemActive()) { m_activeViewport = viewports::VIEWPORT_3D; }

				ImGui::End();

				ImGui::Begin("#editor top", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowSize({ extent.x, extent.y });
				ImGui::SetWindowPos({ topLeft.x + extent.x, topLeft.y });

				if (ImGui::IsItemActive()) { m_activeViewport = viewports::VIEWPORT_TOP; }

				ImGui::End();

				ImGui::Begin("#editor right", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowSize({ extent.x, extent.y });
				ImGui::SetWindowPos({ topLeft.x, topLeft.y + extent.y });

				if (ImGui::IsItemActive()) { m_activeViewport = viewports::VIEWPORT_RIGHT; }

				ImGui::End();

				ImGui::Begin("#editor front", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowSize({ extent.x, extent.y });
				ImGui::SetWindowPos({ topLeft.x + extent.x, topLeft.y + extent.y });

				if (ImGui::IsItemActive()) { m_activeViewport = viewports::VIEWPORT_FRONT; }

				ImGui::End();
			}
		else
			{
				ImGui::Begin("#editor 3d", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
				ImGui::SetWindowSize({ extent.x * 2.f, extent.y * 2.f });
				ImGui::SetWindowPos({ topLeft.x, topLeft.y });

				if (ImGui::IsItemActive()) { m_activeViewport = viewports::VIEWPORT_3D; }

				ImGui::End();
			}
	}

editor::editor(window &window) : m_window(window)
	{
		initKeybinds();
	}

void editor::update()
	{
	}

void editor::fixedUpdate(float deltaTime)
	{
		switch (m_activeViewport) 
			{
				case viewports::VIEWPORT_3D:
					break;
				case viewports::VIEWPORT_FRONT:
					break;
				case viewports::VIEWPORT_RIGHT:
					break;
				case viewports::VIEWPORT_TOP:
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

		drawRightPanel();

		ImGui::End();

		ImGui::Begin("#editor bottom panel", &enabled, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize({ totalSize.x * m_bottomPanelSize.x, totalSize.y * m_bottomPanelSize.y });
		ImGui::SetWindowPos({ 0, barSize.y + totalSize.y - totalSize.y * m_bottomPanelSize.y });

		drawRightPanel();

		drawEditorViewports({ totalSize.x * m_leftPanelSize.x, barSize.y + totalSize.y * m_topPanelSize.y }, { totalSize.x - totalSize.x * m_rightPanelSize.x, barSize.y + totalSize.y - totalSize.y * m_bottomPanelSize.y });

		ImGui::End();
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
