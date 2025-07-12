#include "UIManager.h"
#include "DebugPanel.h"
#include "InfoWindow.h"
#include "ThemePanel.h"
#include "TerminalWindow.h"
#include "LogWindow.h"
#include "TextureViewerWindow.h"
#include "ToolbarWindow.h"
#include "PropertiesWindow.h"
#include "CodeEditorWindow.h"
#include "AddonManagerWindow.h"
#include "NodeEditorWindow.h"
#include "ThemeEditorWindow.h"
#include "StrokeWindow.h"
#include "MainMenuBar.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <windows.h>
#include "../assets/fonts/fontRobotoRegular.h"
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"

namespace blot {

UIManager::UIManager(GLFWwindow* window) : m_window(window) {
    // Create window manager
    m_windowManager = std::make_unique<WindowManager>();
    
    // Create UI panels
    m_debugPanel = std::make_unique<DebugPanel>();
    m_infoWindow = std::make_unique<InfoWindow>();
    m_themePanel = std::make_unique<ThemePanel>();
    
    // Create new windows
    m_terminalWindow = std::make_unique<TerminalWindow>();
    m_logWindow = std::make_unique<LogWindow>();
    
    setupWindows();
    configureWindowSettings();
}

void UIManager::initImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    // Set up ImGui style (Light theme)
    ImGui::StyleColorsLight();
    ImGuiStyle& style = ImGui::GetStyle();
    // Optionally scale UI by monitor DPI (like ofxBapp)
    float uiScale = 1.0f;
#ifdef _WIN32
    // Windows: use GetDpiForSystem or GetDpiForMonitor if available
    HDC screen = GetDC(0);
    int dpi = GetDeviceCaps(screen, LOGPIXELSX);
    ReleaseDC(0, screen);
    uiScale = dpi / 96.0f;
#endif
    style.ScaleAllSizes(uiScale);

    // Load Roboto font from memory
    ImFontConfig font_cfg;
    font_cfg.FontDataOwnedByAtlas = false;
    io.Fonts->AddFontFromMemoryTTF(fontRobotoRegular, sizeof(fontRobotoRegular), 16.0f * uiScale, &font_cfg);
    // Optionally set as default font
    io.FontDefault = io.Fonts->Fonts.back();

    // Load FontAwesome Solid font and merge
    float baseFontSize = 16.0f * uiScale;
    float iconFontSize = baseFontSize * 2.0f / 3.0f;
    static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    ImFontConfig icons_config;
    icons_config.MergeMode = true;
    icons_config.PixelSnapH = true;
    icons_config.GlyphMinAdvanceX = iconFontSize;
    io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", iconFontSize, &icons_config, icons_ranges);

    // Initialize ImGui with GLFW and OpenGL
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    // Initialize enhanced text renderer
    m_textRenderer = std::make_unique<TextRenderer>();
    m_imguiRenderer = std::make_unique<ImGuiRenderer>(m_textRenderer.get());
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
    
    if (m_infoWindow) {
        m_infoWindow->render();
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
    
    auto infoWindow = std::make_shared<InfoWindow>("Info###MainInfo", 
                                                          Window::Flags::AlwaysAutoResize);
    m_windowManager->createWindow("Info", infoWindow);
    
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