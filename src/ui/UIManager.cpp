#include "UIManager.h"
#include "DebugPanel.h"
#include "CoordinateSystemPanel.h"
#include "ThemePanel.h"
#include "TerminalWindow.h"
#include "LogWindow.h"
#include "TextureViewerWindow.h"
#include "ToolbarWindow.h"
#include "CoordinateSystemPanel.h"
#include "PropertiesWindow.h"
#include "CodeEditorWindow.h"
#include "AddonManagerWindow.h"
#include "NodeEditorWindow.h"
#include "ThemeEditorWindow.h"
#include "StrokeWindow.h"
#include <imgui.h>

namespace blot {

UIManager::UIManager() {
    // Create window manager
    m_windowManager = std::make_unique<WindowManager>();
    
    // Create UI panels
    m_debugPanel = std::make_unique<DebugPanel>();
    m_coordinateSystemPanel = std::make_unique<CoordinateSystemPanel>();
    m_themePanel = std::make_unique<ThemePanel>();
    
    // Create new windows
    m_terminalWindow = std::make_unique<TerminalWindow>();
    m_logWindow = std::make_unique<LogWindow>();
    
    setupWindows();
    configureWindowSettings();
}

void UIManager::render() {
    setupDockspace();
    renderAllWindows();
}

void UIManager::handleInput() {
    // Handle global input if needed
    if (m_windowManager) {
        m_windowManager->handleInput();
    }
}

void UIManager::update() {
    // Update UI components
    if (m_windowManager) {
        m_windowManager->update();
    }
}

void UIManager::setupDockspace() {
    // Fullscreen dockspace setup (not nested in another window)
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    ImGui::End();
}

void UIManager::renderAllWindows() {
    // Render all windows through the window manager
    if (m_windowManager) {
        m_windowManager->renderAllWindows();
    }
    
    // Render panels
    if (m_debugPanel) {
        m_debugPanel->render();
    }
    
    if (m_coordinateSystemPanel) {
        m_coordinateSystemPanel->render();
    }
    
    if (m_themePanel) {
        m_themePanel->render();
    }
    
    if (m_terminalWindow) {
        m_terminalWindow->render();
    }
    
    if (m_logWindow) {
        m_logWindow->render();
    }
}

void UIManager::setupWindows() {
    if (!m_windowManager) return;
    
    // Create and register texture viewer window
    auto textureViewerWindow = std::make_shared<TextureViewerWindow>("Texture###MainTexture", 
                                                          Window::Flags::NoScrollbar | Window::Flags::NoCollapse);
    m_windowManager->createWindow("Texture", textureViewerWindow);
    
    // Create and register UI component windows
    auto toolbarWindow = std::make_shared<ToolbarWindow>("Toolbar###MainToolbar", 
                                                      Window::Flags::NoTitleBar | Window::Flags::AlwaysAutoResize);
    m_windowManager->createWindow("Toolbar", toolbarWindow);
    
    auto coordinateSystemPanel = std::make_shared<CoordinateSystemPanel>("Coordinate System###MainCoordinateSystem", 
                                                          Window::Flags::AlwaysAutoResize);
    m_windowManager->createWindow("CoordinateSystem", coordinateSystemPanel);
    
    auto propertiesWindow = std::make_shared<PropertiesWindow>("Properties###MainProperties", 
                                                            Window::Flags::None);
    m_windowManager->createWindow("Properties", propertiesWindow);
    
    // Create and register code editor window
    auto codeEditorWindow = std::make_shared<CodeEditorWindow>("Code Editor###MainCodeEditor", 
                                                           Window::Flags::None);
    m_windowManager->createWindow("CodeEditor", codeEditorWindow);
    
    // Create and register main menu bar
    auto mainMenuBar = std::make_shared<MainMenuBar>("MainMenuBar###MainMenuBar", 
                                                   Window::Flags::None);
    m_windowManager->createWindow("MainMenuBar", mainMenuBar);
    
    // Create and register addon manager window
    auto addonManagerWindow = std::make_shared<AddonManagerWindow>("Addon Manager###AddonManager", 
                                                               Window::Flags::None);
    m_windowManager->createWindow("AddonManager", addonManagerWindow);
    
    // Create and register node editor window
    auto nodeEditorWindow = std::make_shared<NodeEditorWindow>("Node Editor###NodeEditor", 
                                                           Window::Flags::None);
    m_windowManager->createWindow("NodeEditor", nodeEditorWindow);
    
    // Create and register theme editor window
    auto themeEditorWindow = std::make_shared<ThemeEditorWindow>("Theme Editor###ThemeEditor", 
                                                             Window::Flags::None);
    m_windowManager->createWindow("ThemeEditor", themeEditorWindow);
    
    // Create and register stroke window
    auto strokeWindow = std::make_shared<StrokeWindow>("Stroke Settings###StrokeWindow", 
                                                    Window::Flags::None);
    m_windowManager->createWindow("StrokeWindow", strokeWindow);
}

void UIManager::configureWindowSettings() {
    if (!m_windowManager) return;
    
    // Configure window settings
    WindowSettingsComponent toolbarSettings;
    toolbarSettings.category = "Tools";
    toolbarSettings.showByDefault = true;
    toolbarSettings.showInMenu = true;
    m_windowManager->setWindowSettings("Toolbar", toolbarSettings);
    
    WindowSettingsComponent coordinateSystemSettings;
    coordinateSystemSettings.category = "Debug";
    coordinateSystemSettings.showByDefault = true;
    coordinateSystemSettings.showInMenu = true;
    m_windowManager->setWindowSettings("CoordinateSystem", coordinateSystemSettings);
    
    WindowSettingsComponent propertiesSettings;
    propertiesSettings.category = "Tools";
    propertiesSettings.showByDefault = false;
    propertiesSettings.showInMenu = true;
    m_windowManager->setWindowSettings("Properties", propertiesSettings);
    
    WindowSettingsComponent canvasSettings;
    canvasSettings.category = "Main";
    canvasSettings.showByDefault = true;
    canvasSettings.showInMenu = true;
    m_windowManager->setWindowSettings("Canvas", canvasSettings);
    
    WindowSettingsComponent codeEditorSettings;
    codeEditorSettings.category = "Tools";
    codeEditorSettings.showByDefault = true;
    codeEditorSettings.showInMenu = true;
    m_windowManager->setWindowSettings("CodeEditor", codeEditorSettings);
    
    WindowSettingsComponent mainMenuBarSettings;
    mainMenuBarSettings.category = "Main";
    mainMenuBarSettings.showByDefault = true;
    mainMenuBarSettings.showInMenu = false;
    m_windowManager->setWindowSettings("MainMenuBar", mainMenuBarSettings);
    
    WindowSettingsComponent addonManagerSettings;
    addonManagerSettings.category = "Tools";
    addonManagerSettings.showByDefault = false;
    addonManagerSettings.showInMenu = true;
    m_windowManager->setWindowSettings("AddonManager", addonManagerSettings);
    
    WindowSettingsComponent nodeEditorSettings;
    nodeEditorSettings.category = "Tools";
    nodeEditorSettings.showByDefault = false;
    nodeEditorSettings.showInMenu = true;
    m_windowManager->setWindowSettings("NodeEditor", nodeEditorSettings);
    
    WindowSettingsComponent themeEditorSettings;
    themeEditorSettings.category = "Tools";
    themeEditorSettings.showByDefault = false;
    themeEditorSettings.showInMenu = true;
    m_windowManager->setWindowSettings("ThemeEditor", themeEditorSettings);
    
    WindowSettingsComponent strokeWindowSettings;
    strokeWindowSettings.category = "Tools";
    strokeWindowSettings.showByDefault = false;
    strokeWindowSettings.showInMenu = true;
    m_windowManager->setWindowSettings("StrokeWindow", strokeWindowSettings);
}

} // namespace blot 