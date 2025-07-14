#include "MainMenuBar.h"
#include "CodeEditorWindow.h"
#include "../../ui/UIManager.h"
#include <imgui.h>
#include <iostream>
#include "canvas/Canvas.h"

namespace blot {

MainMenuBar::MainMenuBar(const std::string& title)
    : m_title(title) {
}

void MainMenuBar::triggerAction(const std::string& actionId) {
    if (m_eventSystem) {
        m_eventSystem->triggerAction(actionId);
    }
}

bool MainMenuBar::hasAction(const std::string& actionId) const {
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
                if (hasAction("get_all_windows") && hasAction("get_window_visibility") && hasAction("set_window_visibility")) {
                    // Trigger actions to get window data
                    triggerAction("get_all_windows");
                    // Note: In a real implementation, you'd need to handle the return values
                    // This is a simplified version
                } else {
                    ImGui::Text("Window management not available");
                }
                ImGui::EndMenu();
            }
            
            // Theme submenu
            if (ImGui::BeginMenu("Theme")) {
                if (hasAction("switch_theme")) {
                    // Get current theme from UIManager (global ImGui theme)
                    int currentTheme = 0; // Default to Dark
                    if (m_uiManager) {
                        currentTheme = static_cast<int>(m_uiManager->getImGuiTheme());
                    }
                    
                    if (ImGui::MenuItem("Dark", nullptr, currentTheme == 0)) {
                        m_eventSystem->triggerAction("switch_theme", 0);
                    }
                    if (ImGui::MenuItem("Light", nullptr, currentTheme == 1)) {
                        m_eventSystem->triggerAction("switch_theme", 1);
                    }
                    if (ImGui::MenuItem("Classic", nullptr, currentTheme == 2)) {
                        m_eventSystem->triggerAction("switch_theme", 2);
                    }
                    if (ImGui::MenuItem("Corporate", nullptr, currentTheme == 3)) {
                        m_eventSystem->triggerAction("switch_theme", 3);
                    }
                    if (ImGui::MenuItem("Dracula", nullptr, currentTheme == 4)) {
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
            
            // Canvas list using CanvasManager
            if (m_canvasManager) {
                auto canvasInfo = m_canvasManager->getAllCanvasInfo();
                size_t activeIndex = m_canvasManager->getActiveCanvasIndex();
                
                for (const auto& canvasPair : canvasInfo) {
                    size_t index = canvasPair.first;
                    const std::string& name = canvasPair.second;
                    bool isActive = (index == activeIndex);
                    if (ImGui::MenuItem(name.c_str(), nullptr, isActive)) {
                        m_eventSystem->triggerAction("switch_canvas", static_cast<uint32_t>(index));
                    }
                    if (ImGui::IsItemClicked(1)) { // Right click
                        if (ImGui::BeginPopupContextItem()) {
                            if (ImGui::MenuItem("Close") && m_canvasManager->getCanvasCount() > 1) {
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
        if (ImGui::BeginMenu("Workspace")) {
            // Load submenu
            if (ImGui::BeginMenu("Load")) {
                if (hasAction("get_available_workspaces") && hasAction("load_workspace")) {
                    // In a real implementation, you'd get the workspaces and display them
                    ImGui::Text("Workspace loading available");
                } else {
                    ImGui::Text("Workspace loading not available");
                }
                ImGui::EndMenu();
            }
            
            // Save submenu
            if (ImGui::BeginMenu("Save")) {
                if (hasAction("get_current_workspace") && hasAction("save_workspace")) {
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
                if (ImGui::MenuItem("Blend2D", nullptr, currentType == RendererType::Blend2D)) {
                    m_eventSystem->triggerAction("switch_renderer", static_cast<int>(RendererType::Blend2D));
                }
                if (ImGui::MenuItem("OpenGL", nullptr, currentType == RendererType::OpenGL)) {
                    m_eventSystem->triggerAction("switch_renderer", static_cast<int>(RendererType::OpenGL));
                }
            } else {
                ImGui::Text("Renderer switching not available");
            }
            ImGui::EndMenu();
        }
        
        // Debug menu
        if (ImGui::BeginMenu("Debug")) {
            if (hasAction("get_debug_mode") && hasAction("set_debug_mode")) {
                // In a real implementation, you'd get the current debug mode
                if (ImGui::MenuItem("Debug Mode", nullptr, false)) {
                    m_eventSystem->triggerAction("set_debug_mode", true);
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