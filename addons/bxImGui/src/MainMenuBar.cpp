#include "MainMenuBar.h"
#include <imgui.h>
#include <iostream>
#include "CodeEditorWindow.h"
#include "Mui.h"
#include "core/BlotEngine.h"
#include "core/canvas/Canvas.h"

namespace blot {

MainMenuBar::MainMenuBar(const std::string &title) : m_title(title) {}

void MainMenuBar::triggerAction(const std::string &actionId) {
	if (m_eventSystem) {
		m_eventSystem->triggerAction(actionId);
	}
}

void MainMenuBar::setEventSystem(ecs::SEvent *eventSystem) {
	m_eventSystem = eventSystem;
	std::cout << "[MainMenuBar] setEventSystem ptr: " << eventSystem
			  << std::endl;
}

bool MainMenuBar::hasAction(const std::string &actionId) const {
	std::cout << "[MainMenuBar] hasAction called, m_eventSystem ptr: "
			  << m_eventSystem << std::endl;
	return m_eventSystem ? m_eventSystem->hasAction(actionId) : false;
}

void MainMenuBar::render() {
	if (ImGui::BeginMainMenuBar()) {
		// File menu
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New Sketch")) {
				triggerAction("new_sketch");
			}
			if (ImGui::MenuItem("Open Sketch")) {
				triggerAction("open_sketch");
			}
			if (ImGui::MenuItem("Save Sketch")) {
				triggerAction("save_sketch");
			}
			ImGui::Separator();
			if (ImGui::MenuItem("Quit")) {
				triggerAction("quit");
			}
			ImGui::EndMenu();
		}

		// Edit menu
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Addon Manager")) {
				triggerAction("addon_manager");
			}
			if (ImGui::MenuItem("Reload Addons")) {
				triggerAction("reload_addons");
			}
			ImGui::EndMenu();
		}

		// View menu
		if (ImGui::BeginMenu("View")) {
			// Window visibility submenu
			if (ImGui::BeginMenu("Windows")) {
				if (m_uiManager && m_uiManager->getWindowManager()) {
					auto windowManager = m_uiManager->getWindowManager();
					auto windowNames = windowManager->getAllWindowNames();
					for (const auto &win : windowNames) {
						bool isVisible = windowManager->isWindowVisible(win);
						if (ImGui::MenuItem(win.c_str(), nullptr, isVisible)) {
							windowManager->setWindowVisible(win, !isVisible);
							isVisible = !isVisible; // Immediate update for UI
						}
					}
					if (windowNames.empty()) {
						ImGui::Text("No windows found");
					}
				} else {
					ImGui::Text("Window management not available");
				}
				ImGui::EndMenu();
			}

			// Theme submenu
			if (ImGui::BeginMenu("Theme")) {
				if (hasAction("switch_theme")) {
					// Get current theme from UI Manager (global ImGui theme)
					int currentTheme = 0; // Default to Dark
					if (m_uiManager) {
						currentTheme =
							static_cast<int>(m_uiManager->getImGuiTheme());
					}

					if (ImGui::MenuItem("Dark", nullptr, currentTheme == 0)) {
						m_eventSystem->triggerAction("switch_theme", 0);
					}
					if (ImGui::MenuItem("Light", nullptr, currentTheme == 1)) {
						m_eventSystem->triggerAction("switch_theme", 1);
					}
					if (ImGui::MenuItem("Classic", nullptr,
										currentTheme == 2)) {
						m_eventSystem->triggerAction("switch_theme", 2);
					}
					if (ImGui::MenuItem("Corporate", nullptr,
										currentTheme == 3)) {
						m_eventSystem->triggerAction("switch_theme", 3);
					}
					if (ImGui::MenuItem("Dracula", nullptr,
										currentTheme == 4)) {
						m_eventSystem->triggerAction("switch_theme", 4);
					}
				} else {
					ImGui::Text("Theme switching not available");
				}
				ImGui::Separator();
				if (ImGui::MenuItem("Theme Editor...")) {
					triggerAction("theme_editor");
				}
				ImGui::EndMenu();
			}

			// Demo windows
			if (ImGui::MenuItem("ImPlot Demo")) {
				triggerAction("implot_demo");
			}
			if (ImGui::MenuItem("ImGui Markdown Demo")) {
				triggerAction("imgui_markdown_demo");
			}
			if (ImGui::MenuItem("Markdown Editor")) {
				triggerAction("markdown_editor");
			}
			ImGui::EndMenu();
		}

		// Canvas menu
		if (ImGui::BeginMenu("Canvas")) {
			if (ImGui::MenuItem("New Canvas")) {
				triggerAction("new_canvas");
			}
			if (ImGui::MenuItem("Save Canvas")) {
				triggerAction("save_canvas");
			}
			ImGui::Separator();

			// List canvases via event system
			if (hasAction("get_canvas_list") && hasAction("get_active_canvas") &&
				hasAction("get_canvas_count")) {
				auto canvasInfo = m_eventSystem->triggerActionWithResult<
					std::vector<std::pair<size_t, std::string>>>(
					"get_canvas_list");
				size_t activeIndex = m_eventSystem->triggerActionWithResult<size_t>(
					"get_active_canvas");
				size_t canvasCount = m_eventSystem->triggerActionWithResult<size_t>(
					"get_canvas_count");

				for (const auto &canvasPair : canvasInfo) {
					size_t index = canvasPair.first;
					const std::string &name = canvasPair.second;
					bool isActive = (index == activeIndex);
					if (ImGui::MenuItem(name.c_str(), nullptr, isActive)) {
						m_eventSystem->triggerAction("switch_canvas",
									 	static_cast<uint32_t>(index));
					}
					if (ImGui::IsItemClicked(1)) {
						if (ImGui::BeginPopupContextItem()) {
							if (ImGui::MenuItem("Close") && canvasCount > 1) {
								m_eventSystem->triggerAction("close_active_canvas");
							}
							ImGui::EndPopup();
						}
					}
				}
			} else {
				ImGui::Text("Canvas management not available");
			}
			ImGui::EndMenu();
		}

		// Workspace menu
		std::cout << "[MainMenuBar] hasAction(get_available_workspaces): "
				  << hasAction("get_available_workspaces") << std::endl;
		std::cout << "[MainMenuBar] hasAction(load_workspace): "
				  << hasAction("load_workspace") << std::endl;
		if (ImGui::BeginMenu("Workspace")) {
			// Load submenu
			if (ImGui::BeginMenu("Load")) {
				if (hasAction("get_available_workspaces") &&
					hasAction("load_workspace")) {
					// Get the list of workspaces from the event system
					if (m_eventSystem) {
						auto workspaces =
							m_eventSystem->triggerActionWithResult<
								std::vector<std::string>>(
								"get_available_workspaces");
						for (const auto &ws : workspaces) {
							if (ImGui::MenuItem(ws.c_str())) {
								std::cout
									<< "[MainMenuBar] MenuItem clicked: " << ws
									<< std::endl;
								m_eventSystem->triggerAction("load_workspace",
															 ws);
							}
						}
						if (workspaces.empty()) {
							ImGui::Text("No workspaces found");
						}
					}
				} else {
					ImGui::Text("Workspace loading not available");
				}
				ImGui::EndMenu();
			}

			// Save submenu
			if (ImGui::BeginMenu("Save")) {
				if (hasAction("get_current_workspace") &&
					hasAction("save_workspace")) {
					if (ImGui::MenuItem("Save Current Workspace...")) {
						triggerAction("save_current_workspace");
					}
				} else {
					ImGui::Text("Workspace saving not available");
				}

				if (hasAction("show_save_workspace_dialog")) {
					if (ImGui::MenuItem("Save Workspace As...")) {
						triggerAction("show_save_workspace_dialog");
					}
				} else {
					ImGui::Text("Save dialog not available");
				}
				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}

		// Run menu
		if (ImGui::BeginMenu("Run")) {
			if (ImGui::MenuItem("Run Sketch")) {
				triggerAction("run_sketch");
			}
			if (ImGui::MenuItem("Stop")) {
				triggerAction("stop_sketch");
			}
			ImGui::EndMenu();
		}

		// Renderer menu
		if (ImGui::BeginMenu("Renderer")) {
			if (hasAction("switch_renderer")) {
				RendererType currentType = RendererType::Blend2D;
				if (m_canvas) {
					currentType = m_canvas->getRendererType();
				}
				if (ImGui::MenuItem("Blend2D", nullptr,
									currentType == RendererType::Blend2D)) {
					m_eventSystem->triggerAction(
						"switch_renderer",
						static_cast<int>(RendererType::Blend2D));
				}
				if (ImGui::MenuItem("OpenGL", nullptr,
									currentType == RendererType::OpenGL)) {
					m_eventSystem->triggerAction(
						"switch_renderer",
						static_cast<int>(RendererType::OpenGL));
				}
			} else {
				ImGui::Text("Renderer switching not available");
			}
			ImGui::EndMenu();
		}

		// Debug menu
		if (ImGui::BeginMenu("Debug")) {
			if (m_uiManager) {
				bool debugMode = m_uiManager->getBlotEngine()->getDebugMode();
				if (ImGui::MenuItem("Debug Mode", nullptr, debugMode)) {
					m_uiManager->getBlotEngine()->setDebugMode(!debugMode);
				}
			} else {
				ImGui::Text("Debug mode not available");
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}

} // namespace blot
