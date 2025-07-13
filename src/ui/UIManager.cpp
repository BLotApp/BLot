#include "UIManager.h"
#include "windows/DebugPanel.h"
#include "windows/InfoWindow.h"
#include "windows/ThemePanel.h"
#include "windows/TerminalWindow.h"
#include "windows/LogWindow.h"
#include "windows/TextureViewerWindow.h"
#include "windows/ToolbarWindow.h"
#include "windows/PropertiesWindow.h"
#include "windows/CodeEditorWindow.h"
#include "windows/AddonManagerWindow.h"
#include "windows/NodeEditorWindow.h"
#include "windows/ThemeEditorWindow.h"
#include "windows/StrokeWindow.h"
#include "windows/MainMenuBar.h"
#include "windows/SaveWorkspaceDialog.h"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <windows.h>
#include "../assets/fonts/fontRobotoRegular.h"
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

namespace blot {

UIManager::UIManager(GLFWwindow* window) : m_window(window) {
    // Create window manager
    m_windowManager = std::make_unique<WindowManager>();
    
    // Create workspace manager
    m_workspaceManager = std::make_unique<WorkspaceManager>();
    
    // Connect WindowManager to WorkspaceManager
    m_workspaceManager->setWindowManager(m_windowManager.get());
    
    // Create UI panels
    m_debugPanel = std::make_unique<DebugPanel>();
    m_infoWindow = std::make_unique<InfoWindow>();
    m_themePanel = std::make_unique<ThemePanel>();
    
    // Create new windows
    m_terminalWindow = std::make_unique<TerminalWindow>();
    m_logWindow = std::make_unique<LogWindow>();
    m_saveWorkspaceDialog = std::make_unique<SaveWorkspaceDialog>("Save Workspace");
    
    m_currentTheme = ImGuiTheme::Light;
    
    setupWindows();
    configureWindowSettings();
}

UIManager::~UIManager() {
    shutdownImGui();
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

void UIManager::shutdownImGui() {
    // Shutdown ImGui implementation
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UIManager::update() {
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Load workspace on first frame only
    static bool workspaceLoaded = false;
    if (!workspaceLoaded) {
        loadWorkspace("current");
        workspaceLoaded = true;
    }
    
    // Simple dockspace setup
    setupDockspace();
    
    // Show debug menu bar
    if (m_debugMode) {
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("Debug")) {
                if (ImGui::MenuItem("Exit Debug Mode")) {
                    m_debugMode = false;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    
    // Update UI components
    if (m_windowManager) {
        m_windowManager->update();
    }
    
    // Render all windows
    renderAllWindows();
    
    // Handle debug mode toggle with F12 key (using GLFW directly)
    static bool f12Pressed = false;
    if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F12) == GLFW_PRESS) {
        if (!f12Pressed) {
            m_debugMode = !m_debugMode;
            f12Pressed = true;
        }
    } else {
        f12Pressed = false;
    }
    
    // Render ImGui frame
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    
    // Update and render additional viewports
    if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

void UIManager::handleInput() {
    // Handle global input if needed
    if (m_windowManager) {
        m_windowManager->handleInput();
    }
}

void UIManager::setupDockspace() {
    // Simplified dockspace setup for debugging
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    
    // Debug: Add some text to the dockspace to see if it's rendering
    ImGui::Begin("DockSpace", nullptr, window_flags);
    ImGui::PopStyleVar(2);
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
    
    // Debug: Add text to dockspace
    ImGui::Text("Dockspace is working!");
    
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
    
    // Create toolbar window
    auto toolbarWindow = std::make_shared<ToolbarWindow>("Toolbar###MainToolbar", Window::Flags::None);
    m_windowManager->createWindow("Toolbar", toolbarWindow);
    
    // Connect theme panel to toolbar window
    if (m_themePanel) {
        m_themePanel->setToolbarWindow(toolbarWindow);
    }
    
    // Create info window
    auto infoWindow = std::make_shared<InfoWindow>("Info Window###MainInfoWindow", 
                                                  Window::Flags::AlwaysAutoResize);
    m_windowManager->createWindow("InfoWindow", infoWindow);
    
    // Create properties window
    auto propertiesWindow = std::make_shared<PropertiesWindow>("Properties###MainProperties", 
                                                             Window::Flags::None);
    m_windowManager->createWindow("Properties", propertiesWindow);
    
    // Create code editor window
    auto codeEditorWindow = std::make_shared<CodeEditorWindow>("Code Editor###MainCodeEditor", 
                                                              Window::Flags::None);
    m_windowManager->createWindow("CodeEditor", codeEditorWindow);
    
    // Create main menu bar
    auto mainMenuBar = std::make_shared<MainMenuBar>("MainMenuBar###MainMenuBar", 
                                                     Window::Flags::None);
    m_windowManager->createWindow("MainMenuBar", mainMenuBar);
    
    // Create addon manager window
    auto addonManagerWindow = std::make_shared<AddonManagerWindow>("Addon Manager###AddonManager", 
                                                                  Window::Flags::None);
    m_windowManager->createWindow("AddonManager", addonManagerWindow);
    
    // Create node editor window
    auto nodeEditorWindow = std::make_shared<NodeEditorWindow>("Node Editor###NodeEditor", 
                                                              Window::Flags::None);
    m_windowManager->createWindow("NodeEditor", nodeEditorWindow);
    
    // Create stroke window
    auto strokeWindow = std::make_shared<StrokeWindow>("Stroke###StrokeWindow", Window::Flags::None);
    m_windowManager->createWindow("Stroke", strokeWindow);
    
    // Create and register theme editor window
    auto themeEditorWindow = std::make_shared<ThemeEditorWindow>("Theme Editor###ThemeEditor", 
                                                               Window::Flags::None);
    themeEditorWindow->setUIManager(this);
    m_windowManager->createWindow("ThemeEditor", themeEditorWindow);
    
    // Register save workspace dialog (not shown by default)
    auto saveWorkspaceDialog = std::shared_ptr<SaveWorkspaceDialog>(m_saveWorkspaceDialog.get(), [](SaveWorkspaceDialog*){});
    m_windowManager->createWindow("SaveWorkspaceDialog", saveWorkspaceDialog);
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
    
    WindowSettingsComponent saveWorkspaceDialogSettings;
    saveWorkspaceDialogSettings.category = "Dialogs";
    saveWorkspaceDialogSettings.showByDefault = false;
    saveWorkspaceDialogSettings.showInMenu = false; // Don't show in menu since it's a dialog
    m_windowManager->setWindowSettings("SaveWorkspaceDialog", saveWorkspaceDialogSettings);
    
    // Stroke window settings are configured in BlotApp.cpp
}

void UIManager::setImGuiTheme(ImGuiTheme theme) {
    ImGuiStyle& style = ImGui::GetStyle();
    switch (theme) {
        case ImGuiTheme::Dark:
            ImGui::StyleColorsDark();
            break;
        case ImGuiTheme::Light:
            ImGui::StyleColorsLight();
            break;
        case ImGuiTheme::Classic:
            ImGui::StyleColorsClassic();
            break;
        case ImGuiTheme::Corporate: {
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.34f, 0.39f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
            break;
        }
        case ImGuiTheme::Dracula: {
            ImGui::StyleColorsDark();
            style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.10f, 1.00f);
            style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
            style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.50f, 0.28f, 0.92f, 1.00f);
            style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
            style.Colors[ImGuiCol_Button] = ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
            style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.28f, 0.92f, 1.00f);
            style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
            style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.15f, 0.30f, 1.00f);
            style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
            style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
            break;
        }
    }
    m_currentTheme = theme;
}

void UIManager::loadTheme(const std::string& path) {
    ImGuiStyle& style = ImGui::GetStyle();
    std::ifstream in(path);
    if (!in) return;
    nlohmann::json j;
    in >> j;
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        auto it = j.find(ImGui::GetStyleColorName(i));
        if (it != j.end() && it->is_array() && it->size() == 4) {
            style.Colors[i] = ImVec4((*it)[0].get<float>(), (*it)[1].get<float>(), (*it)[2].get<float>(), (*it)[3].get<float>());
        }
    }
}

void UIManager::saveCurrentTheme(const std::string& path) {
    ImGuiStyle& style = ImGui::GetStyle();
    nlohmann::json j;
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        j[ImGui::GetStyleColorName(i)] = {
            style.Colors[i].x, style.Colors[i].y, style.Colors[i].z, style.Colors[i].w
        };
    }
    std::ofstream out(path);
    out << j.dump(2);
}

void UIManager::setWindowVisibility(const std::string& windowName, bool visible) {
    if (m_workspaceManager) {
        m_workspaceManager->setWindowVisibility(windowName, visible);
    }
    
    if (m_windowManager) {
        auto window = m_windowManager->getWindow(windowName);
        if (window) {
            if (visible) {
                window->show();
            } else {
                window->hide();
            }
        }
    }
}

void UIManager::setWindowVisibilityAll(bool visible) {
    if (m_windowManager) {
        std::vector<std::string> allWindows = m_windowManager->getAllWindowNames();
        for (const auto& windowName : allWindows) {
            setWindowVisibility(windowName, visible);
        }
    }
}

bool UIManager::getWindowVisibility(const std::string& windowName) const {
    if (m_windowManager) {
        auto window = m_windowManager->getWindow(windowName);
        if (window) {
            return window->isOpen();
        }
    }
    return true;
}

std::vector<std::string> UIManager::getAllWindowNames() const {
    if (m_windowManager) {
        return m_windowManager->getAllWindowNames();
    }
    return {};
}



bool UIManager::loadWorkspace(const std::string& workspaceName) {
    if (m_workspaceManager) {
        std::cout << "Loading workspace: " << workspaceName << std::endl;
        
        // First, hide all windows
        setWindowVisibilityAll(false);
        
        bool success = m_workspaceManager->loadWorkspace(workspaceName);
        if (success) {
            // Force ImGui to update its layout
            ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
            std::cout << "Workspace loaded successfully" << std::endl;
        } else {
            std::cout << "Failed to load workspace" << std::endl;
        }
        return success;
    }
    return false;
}

bool UIManager::saveWorkspace(const std::string& workspaceName) {
    if (m_workspaceManager) {
        return m_workspaceManager->saveWorkspace(workspaceName);
    }
    return false;
}

bool UIManager::saveWorkspaceAs(const std::string& workspaceName) {
    if (m_workspaceManager) {
        return m_workspaceManager->saveWorkspaceAs(workspaceName);
    }
    return false;
}

std::string UIManager::getCurrentWorkspace() const {
    if (m_workspaceManager) {
        return m_workspaceManager->getCurrentWorkspace();
    }
    return "default";
}

std::vector<std::pair<std::string, std::string>> UIManager::getAvailableWorkspaces() const {
    if (m_workspaceManager) {
        return m_workspaceManager->getAvailableWorkspacesWithNames();
    }
    return {};
}

std::vector<std::string> UIManager::getAllWorkspaceNames() const {
    if (m_workspaceManager) {
        return m_workspaceManager->getAvailableWorkspaces();
    }
    return {};
}

void UIManager::setupWindowCallbacks(BlotApp* app) {
    if (!m_windowManager || !app) return;
    
    // Get the main menu bar and set up its callbacks
    auto mainMenuBar = std::dynamic_pointer_cast<MainMenuBar>(m_windowManager->getWindow("MainMenuBar"));
    if (mainMenuBar) {
        // Configure main menu bar callbacks
        mainMenuBar->setQuitCallback([app]() {
            // This will be handled by the app's main loop
        });
        
        mainMenuBar->setAddonManagerCallback([this]() {
            auto addonManagerWindow = m_windowManager->getWindow("AddonManager");
            if (addonManagerWindow) {
                addonManagerWindow->show();
            }
        });
        
        mainMenuBar->setReloadAddonsCallback([app]() {
            // This will be handled by the app
        });
        
        mainMenuBar->setThemeEditorCallback([this]() {
            auto themeEditorWindow = m_windowManager->getWindow("ThemeEditor");
            if (themeEditorWindow) {
                themeEditorWindow->show();
            }
        });
        
        // Window visibility management callbacks
        mainMenuBar->setWindowVisibilityCallback([this](const std::string& windowName, bool visible) {
            setWindowVisibility(windowName, visible);
        });
        
        mainMenuBar->setGetWindowVisibilityCallback([this](const std::string& windowName) {
            return getWindowVisibility(windowName);
        });
        
        mainMenuBar->setGetAllWindowsCallback([this]() {
            return getAllWindowNames();
        });
        
        // Workspace management callbacks
        mainMenuBar->setLoadWorkspaceCallback([this](const std::string& workspaceName) {
            loadWorkspace(workspaceName);
        });
        
        mainMenuBar->setSaveWorkspaceCallback([this](const std::string& workspaceName) {
            saveWorkspace(workspaceName);
        });
        
        mainMenuBar->setSaveWorkspaceAsCallback([this](const std::string& workspaceName) {
            saveWorkspaceAs(workspaceName);
        });
        
        mainMenuBar->setCurrentWorkspaceCallback([this]() {
            return getCurrentWorkspace();
        });
        
        mainMenuBar->setGetAvailableWorkspacesCallback([this]() {
            std::vector<std::string> workspaceNames = getAllWorkspaceNames();
            std::vector<std::pair<std::string, std::string>> workspaces;
            for (const auto& name : workspaceNames) {
                workspaces.push_back({name, name}); // Use name as both id and display name
            }
            return workspaces;
        });
        
        mainMenuBar->setShowSaveWorkspaceDialogCallback([this]() {
            auto saveDialog = getSaveWorkspaceDialog();
            if (saveDialog) {
                saveDialog->setInitialWorkspaceName(getCurrentWorkspace());
                saveDialog->setSaveCallback([this](const std::string& workspaceName) {
                    saveWorkspaceAs(workspaceName);
                });
                saveDialog->setCancelCallback([this]() {
                    // Dialog will close itself
                });
                saveDialog->show();
            }
        });
        
        // Theme switching callbacks
        mainMenuBar->setSwitchThemeCallback([this](int theme) {
            setImGuiTheme(static_cast<ImGuiTheme>(theme));
            m_currentTheme = static_cast<ImGuiTheme>(theme);
        });
        
        mainMenuBar->setCurrentTheme(static_cast<int>(m_currentTheme));
        
        // Debug mode callbacks
        mainMenuBar->setDebugModeCallback([this](bool enabled) {
            setDebugMode(enabled);
        });
        
        mainMenuBar->setGetDebugModeCallback([this]() {
            return m_debugMode;
        });
    }
    
    // Get the toolbar and set up its callbacks
    auto toolbarWindow = std::dynamic_pointer_cast<ToolbarWindow>(m_windowManager->getWindow("Toolbar"));
    if (toolbarWindow) {
        toolbarWindow->setOnStrokeWidthChanged([this](float width) {
            // Get stroke window through WindowManager
            auto strokeWindow = std::dynamic_pointer_cast<StrokeWindow>(m_windowManager->getWindow("Stroke"));
            if (strokeWindow) {
                strokeWindow->setStrokeWidth(static_cast<double>(width));
            }
        });
    }
    
    // Get the stroke window and set up its callbacks
    auto strokeWindow = std::dynamic_pointer_cast<StrokeWindow>(m_windowManager->getWindow("Stroke"));
    if (strokeWindow) {
        strokeWindow->setStrokeWidthCallback([app](double width) {
            // This will be handled by the app
        });
        
        strokeWindow->setStrokeCapCallback([app](BLStrokeCap cap) {
            // This will be handled by the app
        });
        
        strokeWindow->setStrokeJoinCallback([app](BLStrokeJoin join) {
            // This will be handled by the app
        });
        
        strokeWindow->setMiterLimitCallback([app](double limit) {
            // This will be handled by the app
        });
        
        strokeWindow->setDashArrayCallback([app](const std::vector<double>& dashes) {
            // This will be handled by the app
        });
        
        strokeWindow->setDashOffsetCallback([app](double offset) {
            // This will be handled by the app
        });
        
        strokeWindow->setTransformOrderCallback([app](BLStrokeTransformOrder order) {
            // This will be handled by the app
        });
    }
}

void UIManager::saveCurrentImGuiLayout() {
    if (m_workspaceManager) {
        m_workspaceManager->saveCurrentImGuiLayout();
    }
}

} // namespace blot 