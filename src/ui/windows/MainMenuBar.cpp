#include "MainMenuBar.h"
#include "imgui.h"
#include <entt/entt.hpp>
#include <ctime>

namespace blot {

MainMenuBar::MainMenuBar(const std::string& title, Window::Flags flags) 
    : Window(title, flags) {
}

void MainMenuBar::render() {
    if (ImGui::BeginMainMenuBar()) {
        // BLot menu
        if (ImGui::BeginMenu("BLot")) {
            ImGui::Text("BLot");
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences...")) {
                // TODO: Show preferences window
            }
            ImGui::Separator();
            if (m_getDebugModeCallback && m_debugModeCallback) {
                bool debugMode = m_getDebugModeCallback();
                if (ImGui::MenuItem("Debug Mode", nullptr, debugMode)) {
                    m_debugModeCallback(!debugMode);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit.") && m_quitCallback) {
                m_quitCallback();
            }
            
            // Samples submenu
            if (ImGui::BeginMenu("Samples")) {
                if (ImGui::MenuItem("ImPlot") && m_imPlotDemoCallback) {
                    m_imPlotDemoCallback();
                }
                if (ImGui::MenuItem("ImGui Markdown Demo") && m_imGuiMarkdownDemoCallback) {
                    m_imGuiMarkdownDemoCallback();
                }
                if (ImGui::MenuItem("Markdown Editor/Viewer") && m_markdownEditorCallback) {
                    m_markdownEditorCallback();
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        
        // File menu
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Sketch") && m_newSketchCallback) {
                m_newSketchCallback();
            }
            if (ImGui::MenuItem("Open Sketch") && m_openSketchCallback) {
                m_openSketchCallback();
            }
            if (ImGui::MenuItem("Save Sketch") && m_saveSketchCallback) {
                m_saveSketchCallback();
            }
            if (ImGui::MenuItem("Save As...") && m_saveCanvasCallback) {
                m_saveCanvasCallback();
            }
            if (ImGui::MenuItem("Open Markdown...") && m_openMarkdownCallback) {
                m_openMarkdownCallback();
            }
            if (ImGui::MenuItem("Save Markdown As...") && m_saveMarkdownCallback) {
                m_saveMarkdownCallback();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit") && m_quitCallback) {
                m_quitCallback();
            }
            ImGui::EndMenu();
        }
        
        // Edit menu
        if (ImGui::BeginMenu("Edit")) {
            // Add edit actions here
            ImGui::EndMenu();
        }
        
        // View menu
        if (ImGui::BeginMenu("View")) {
            // Windows submenu
            if (ImGui::BeginMenu("Windows")) {
                if (m_getAllWindowsCallback) {
                    auto windows = m_getAllWindowsCallback();
                    for (const auto& windowName : windows) {
                        bool isVisible = m_getWindowVisibilityCallback ? m_getWindowVisibilityCallback(windowName) : true;
                        if (ImGui::MenuItem(windowName.c_str(), nullptr, isVisible) && m_windowVisibilityCallback) {
                            m_windowVisibilityCallback(windowName, !isVisible);
                        }
                    }
                }
                ImGui::EndMenu();
            }
            
            ImGui::Separator();
            
            // Workspace submenu
            if (ImGui::BeginMenu("Workspace")) {
                // Load submenu
                if (ImGui::BeginMenu("Load")) {
                    // Built-in workspaces
                    if (ImGui::MenuItem("Creative Coding") && m_loadWorkspaceCallback) {
                        m_loadWorkspaceCallback("creative_coding");
                    }
                    if (ImGui::MenuItem("Illustrator") && m_loadWorkspaceCallback) {
                        m_loadWorkspaceCallback("illustrator");
                    }
                    
                    // Show available custom workspaces
                    if (m_getAvailableWorkspacesCallback) {
                        auto workspaces = m_getAvailableWorkspacesCallback();
                        if (!workspaces.empty()) {
                            ImGui::Separator();
                            for (const auto& [id, name] : workspaces) {
                                // Skip built-in workspaces
                                if (id != "creative_coding" && id != "illustrator") {
                                    if (ImGui::MenuItem(name.c_str()) && m_loadWorkspaceCallback) {
                                        m_loadWorkspaceCallback(id);
                                    }
                                }
                            }
                        }
                    }
                    ImGui::EndMenu();
                }
                
                // Save submenu
                if (ImGui::BeginMenu("Save")) {
                    if (ImGui::MenuItem("Save Current Workspace...") && m_saveWorkspaceCallback) {
                        std::string currentWorkspace = m_getCurrentWorkspaceCallback ? m_getCurrentWorkspaceCallback() : "default";
                        m_saveWorkspaceCallback(currentWorkspace);
                    }
                    if (ImGui::MenuItem("Save Workspace As...") && m_showSaveWorkspaceDialogCallback) {
                        m_showSaveWorkspaceDialogCallback();
                    }
                    ImGui::EndMenu();
                }
                
                ImGui::EndMenu();
            }
            
            ImGui::EndMenu();
        }
        
        // Run menu
        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Run Sketch") && m_runSketchCallback) {
                m_runSketchCallback();
            }
            if (ImGui::MenuItem("Stop") && m_stopSketchCallback) {
                m_stopSketchCallback();
            }
            ImGui::EndMenu();
        }
        
        // Renderer menu
        if (ImGui::BeginMenu("Renderer")) {
            if (ImGui::MenuItem("Blend2D", nullptr, m_currentRendererType == 0) && m_switchRendererCallback) {
                m_switchRendererCallback(0);
            }
            if (ImGui::MenuItem("OpenGL/ES", nullptr, m_currentRendererType == 1) && m_switchRendererCallback) {
                m_switchRendererCallback(1);
            }
            ImGui::EndMenu();
        }
        
        // Canvases menu
        if (ImGui::BeginMenu("Canvases")) {
            if (ImGui::MenuItem("New Canvas") && m_newCanvasCallback) {
                m_newCanvasCallback();
            }
            ImGui::Separator();
            
            // List existing canvases
            for (const auto& [entity, name] : m_canvasEntities) {
                bool isActive = (entity == m_activeCanvasId);
                if (ImGui::Selectable(name.c_str(), isActive) && m_selectCanvasCallback) {
                    m_selectCanvasCallback(entity);
                }
                ImGui::SameLine();
                if (ImGui::SmallButton(("x##close_" + std::to_string((int)entity)).c_str()) && m_closeCanvasCallback) {
                    m_closeCanvasCallback(entity);
                    break;
                }
            }
            ImGui::EndMenu();
        }
        
        // Addons menu
        if (ImGui::BeginMenu("Addons")) {
            if (ImGui::MenuItem("Addon Manager") && m_addonManagerCallback) {
                m_addonManagerCallback();
            }
            if (ImGui::MenuItem("Reload Addons") && m_reloadAddonsCallback) {
                m_reloadAddonsCallback();
            }
            ImGui::EndMenu();
        }
        
        // Theme menu
        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Dark", nullptr, m_currentTheme == 0) && m_switchThemeCallback) {
                m_switchThemeCallback(0);
            }
            if (ImGui::MenuItem("Light", nullptr, m_currentTheme == 1) && m_switchThemeCallback) {
                m_switchThemeCallback(1);
            }
            if (ImGui::MenuItem("Classic", nullptr, m_currentTheme == 2) && m_switchThemeCallback) {
                m_switchThemeCallback(2);
            }
            if (ImGui::MenuItem("Corporate", nullptr, m_currentTheme == 3) && m_switchThemeCallback) {
                m_switchThemeCallback(3);
            }
            if (ImGui::MenuItem("Dracula", nullptr, m_currentTheme == 4) && m_switchThemeCallback) {
                m_switchThemeCallback(4);
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Theme Editor...") && m_themeEditorCallback) {
                m_themeEditorCallback();
            }
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
}

} // namespace blot 