

// Standard library includes
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <string>

// Third-party includes
#include "core/json.h"

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <spdlog/spdlog.h>

// Project includes
#include "core/core.h"
#include "ui/ui.h"
#include "rendering/rendering.h"
#include "app.h"
#include "addons/bxScriptEngine/bxScriptEngine.h"
#include "addons/bxCodeEditor/bxCodeEditor.h"
#include "ui/windows/NodeEditorWindow.h"
#include "ui/windows/CanvasWindow.h"
#include "ecs/ECSManager.h"
#include "ui/CodeEditor.h"
#include "core/canvas/CanvasManager.h"
#include "core/util/SettingsManager.h"
#include "rendering/RenderingManager.h"
#include "ui/UIManager.h"
#include "core/BlotEngine.h"

SampleUiApp::SampleUiApp() 
    : m_window(nullptr)
    , m_windowWidth(1280)
    , m_windowHeight(720)
    , m_running(true)
    , m_deltaTime(0.0f)
    , m_lastFrameTime(0.0f)
{

    // No need to call setup() here; the engine will call it after initialization.

    try {
        // Initialize code editor
        auto codeEditorPtr = getAddonManager()->getAddon("bxCodeEditor");
        auto m_codeEditor = codeEditorPtr ? dynamic_cast<bxCodeEditor*>(codeEditorPtr.get()) : nullptr;
        if (m_codeEditor) {
            m_codeEditor->loadDefaultTemplate();
            spdlog::info("BlotApp: After loadDefaultTemplate");
        }
    } catch (const std::exception& e) {
        spdlog::error("Exception in BlotApp constructor: {}", e.what());
    }
    spdlog::info("BlotApp constructor finished");
}

SampleUiApp::~SampleUiApp() {
    spdlog::info("BlotApp destructor called");
    // Save current ImGui layout
    if (getUIManager()) {
        getUIManager()->saveCurrentImGuiLayout();
    }
}

void SampleUiApp::setup(blot::BlotEngine*) {
    // Final setup phase - everything is now initialized
    spdlog::info("Setting up application...");
    
    // Connect ECS event system to UI components
    if (getUIManager() && getECSManager()) {
        connectEventSystemToUI();
    }
    
    // Run the default sketch on launch
    if (m_codeEditor) {
        if (auto scriptEngine = getScriptEngine()) {
            scriptEngine->runCode(m_codeEditor->getCode());
        }
    }
    
    spdlog::info("Application setup complete");
}

void SampleUiApp::connectEventSystemToUI() {
    // Get the ECS event system
    auto& eventSystem = getECSManager()->getEventSystem();
    
    // Connect MainMenuBar to the ECS event system
    auto mainMenuBar = getUIManager()->getMainMenuBar();
    spdlog::info("[BlotApp] Setting event system on MainMenuBar: 0x{:X} with eventSystem ptr: 0x{:X}", reinterpret_cast<uintptr_t>(mainMenuBar), reinterpret_cast<uintptr_t>(&eventSystem));
    if (mainMenuBar) {
        mainMenuBar->setEventSystem(&eventSystem);
        
        // Connect MainMenuBar to CodeEditorWindow for theme state
        auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
            getUIManager()->getWindowManager()->getWindow("CodeEditor"));
        if (codeEditorWindow) {
            mainMenuBar->setCodeEditorWindow(codeEditorWindow);
        }
        
        // Connect MainMenuBar to CanvasManager for canvas operations
        if (getCanvasManager()) {
            auto activeCanvas = getCanvasManager()->getActiveCanvas();
            if (activeCanvas) {
                mainMenuBar->setCanvas(activeCanvas);
            }
            mainMenuBar->setCanvasManager(getCanvasManager());
        }
    }
    
    // Connect MainMenuBar to UIManager for ImGui theme
    if (mainMenuBar && getUIManager()) {
        mainMenuBar->setUIManager(getUIManager().get());
    }
    
    // Connect NodeEditorWindow to the ECS system
    auto nodeEditorWindow = std::dynamic_pointer_cast<blot::NodeEditorWindow>(
        getUIManager()->getWindowManager()->getWindow("NodeEditor"));
    if (nodeEditorWindow) {
        // Create a shared_ptr from the central ECSManager for the window
        auto ecsSharedPtr = std::shared_ptr<ECSManager>(getECSManager(), [](ECSManager*){});
        nodeEditorWindow->setECSManager(ecsSharedPtr);
    }
    
    // Connect PropertiesWindow to the ECS system
    auto propertiesWindow = std::dynamic_pointer_cast<blot::PropertiesWindow>(
        getUIManager()->getWindowManager()->getWindow("Properties"));
    if (propertiesWindow) {
        // Create a shared_ptr from the central ECSManager for the window
        auto ecsSharedPtr = std::shared_ptr<ECSManager>(getECSManager(), [](ECSManager*){});
        propertiesWindow->setECSManager(ecsSharedPtr);
    }
    
    // Connect CanvasWindow to the ECS system
    auto canvasWindow = std::dynamic_pointer_cast<blot::CanvasWindow>(
        getUIManager()->getWindowManager()->getWindow("Canvas"));
    if (canvasWindow) {
        canvasWindow->setECSManager(getECSManager());
        canvasWindow->setRenderingManager(getRenderingManager());
        canvasWindow->setActiveCanvasId(m_activeCanvasId);
    }
    
    // Connect AddonManagerWindow to the AddonManager
    auto addonManagerWindow = std::dynamic_pointer_cast<blot::AddonManagerWindow>(
        getUIManager()->getWindowManager()->getWindow("AddonManager"));
    
    // Connect CodeEditorWindow to the theme system
    auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
        getUIManager()->getWindowManager()->getWindow("CodeEditor"));
    if (codeEditorWindow) {
        // Set the current theme state in CodeEditorWindow
        codeEditorWindow->setCurrentTheme(static_cast<int>(getUIManager()->m_currentTheme));
    }
    
    // Register UI actions with the ECS event system
    registerUIActions(eventSystem);
}

void SampleUiApp::connectAddonManagerToEventSystem(blot::systems::EventSystem& eventSystem) {
    // Register addon-related events with the ECS event system
    eventSystem.registerEvent("addon_loaded", [this](const blot::systems::Event& event) {
        spdlog::info("Addon loaded event: {}", event.actionId);
        // Trigger addon manager's global event
        // The AddonManager is now managed by BlotEngine, so we don't need to trigger global events here.
    });
    
    eventSystem.registerEvent("addon_unloaded", [this](const blot::systems::Event& event) {
        spdlog::info("Addon unloaded event: {}", event.actionId);
        // The AddonManager is now managed by BlotEngine, so we don't need to trigger global events here.
    });
    
    eventSystem.registerEvent("addon_enabled", [this](const blot::systems::Event& event) {
        spdlog::info("Addon enabled event: {}", event.actionId);
        // The AddonManager is now managed by BlotEngine, so we don't need to trigger global events here.
    });
    
    eventSystem.registerEvent("addon_disabled", [this](const blot::systems::Event& event) {
        spdlog::info("Addon disabled event: {}", event.actionId);
        // The AddonManager is now managed by BlotEngine, so we don't need to trigger global events here.
    });
}

void SampleUiApp::registerUIActions(blot::systems::EventSystem& eventSystem) {
    spdlog::info("[BlotApp] registerUIActions eventSystem ptr: 0x{:X}", reinterpret_cast<uintptr_t>(&eventSystem));
    // File menu actions
    eventSystem.registerAction("new_sketch", [this]() {
        spdlog::info("New sketch action triggered");
        // TODO: Implement new sketch functionality
    });
    
    eventSystem.registerAction("open_sketch", [this]() {
        spdlog::info("Open sketch action triggered");
        // TODO: Implement open sketch functionality
    });
    
    eventSystem.registerAction("save_sketch", [this]() {
        spdlog::info("Save sketch action triggered");
        // TODO: Implement save sketch functionality
    });
    
    eventSystem.registerAction("quit", [this]() {
        spdlog::info("Quit action triggered");
        m_running = false;
    });
    
    // Edit menu actions
    eventSystem.registerAction("addon_manager", [this]() {
        spdlog::info("Addon manager action triggered");
        auto addonManagerWindow = getUIManager()->getWindowManager()->getWindow("AddonManager");
        if (addonManagerWindow) {
            addonManagerWindow->show();
        }
    });
    
    eventSystem.registerAction("reload_addons", [this]() {
        spdlog::info("Reload addons action triggered");
        // The AddonManager is now managed by BlotApp, so we don't need to reload here.
    });
    
    // View menu actions
    eventSystem.registerAction("theme_editor", [this]() {
        spdlog::info("Theme editor action triggered");
        auto themeEditorWindow = getUIManager()->getWindowManager()->getWindow("ThemeEditor");
        if (themeEditorWindow) {
            themeEditorWindow->show();
        }
    });
    
    eventSystem.registerAction("implot_demo", [this]() {
        spdlog::info("ImPlot demo action triggered");
        // TODO: Implement ImPlot demo
    });
    
    eventSystem.registerAction("imgui_markdown_demo", [this]() {
        spdlog::info("ImGui markdown demo action triggered");
        // TODO: Implement markdown demo
    });
    
    eventSystem.registerAction("markdown_editor", [this]() {
        spdlog::info("Markdown editor action triggered");
        // TODO: Implement markdown editor
    });
    
    // Canvas menu actions
    eventSystem.registerAction("new_canvas", [this]() {
        spdlog::info("New canvas action triggered");
        // TODO: Implement new canvas functionality
    });
    
    eventSystem.registerAction("save_canvas", [this]() {
        spdlog::info("Save canvas action triggered");
        // TODO: Implement save canvas functionality
    });
    
    // Workspace menu actions
    eventSystem.registerAction("save_current_workspace", [this]() {
        spdlog::info("Save current workspace action triggered");
        if (getUIManager()) {
            getUIManager()->saveWorkspace("current");
        }
    });
    
    eventSystem.registerAction("show_save_workspace_dialog", [this]() {
        spdlog::info("Show save workspace dialog action triggered");
        auto saveDialog = getUIManager()->getWindowManager()->getWindow("SaveWorkspaceDialog");
        if (saveDialog) {
            saveDialog->show();
        }
    });
    
    // Run menu actions
    eventSystem.registerAction("run_sketch", [this]() {
        spdlog::info("Run sketch action triggered");
        if (m_codeEditor) {
            if (auto scriptEngine = getScriptEngine()) {
                scriptEngine->runCode(m_codeEditor->getCode());
            }
        }
    });
    
    eventSystem.registerAction("stop_sketch", [this]() {
        spdlog::info("Stop sketch action triggered");
        // TODO: Implement stop sketch functionality
    });
    
    // Theme switching actions
    eventSystem.registerAction("switch_theme", std::function<void(int)>([this](int theme) {
        spdlog::info("Switch theme action triggered: {}", theme);
        if (getUIManager()) {
            getUIManager()->setImGuiTheme(static_cast<blot::UIManager::ImGuiTheme>(theme));
            
            // Update CodeEditorWindow theme state
            auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
                getUIManager()->getWindowManager()->getWindow("CodeEditor"));
            if (codeEditorWindow) {
                codeEditorWindow->setCurrentTheme(theme);
            }
        }
    }));
    
    // Renderer switching actions
    eventSystem.registerAction("switch_renderer", std::function<void(int)>([this](int rendererType) {
        spdlog::info("Switch renderer action triggered: {}", rendererType);
        // Switch renderer for the active canvas
        if (getRenderingManager()) {
            auto canvasPtr = getRenderingManager()->getCanvas(m_activeCanvasId);
            if (canvasPtr && *canvasPtr) {
                (*canvasPtr)->switchRenderer(static_cast<RendererType>(rendererType));
            }
        }
    }));
    
    // Debug actions
    eventSystem.registerAction("set_debug_mode", std::function<void(bool)>([this](bool enabled) {
        spdlog::info("Set debug mode action triggered: {}", enabled);
        blot::BlotEngine::setDebugMode(enabled);
    }));
    
    // Canvas management actions
    eventSystem.registerAction("select_canvas", std::function<void(uint32_t)>([this](uint32_t canvasId) {
        spdlog::info("Select canvas action triggered: {}", canvasId);
        // TODO: Implement canvas selection
    }));
    
    eventSystem.registerAction("close_canvas", std::function<void(uint32_t)>([this](uint32_t canvasId) {
        spdlog::info("Close canvas action triggered: {}", canvasId);
        // TODO: Implement canvas closing
    }));
    
    // Canvas management actions using CanvasManager
    eventSystem.registerAction("new_canvas", std::function<void()>([this]() {
        spdlog::info("New canvas action triggered");
        if (getCanvasManager() && getECSManager()) {
            // Create new ECS canvas entity only
            entt::entity newCanvasEntity = getCanvasManager()->createCanvas(*getECSManager(), m_windowWidth, m_windowHeight);
            m_activeCanvasId = newCanvasEntity;
            spdlog::info("Created new canvas entity: {}", (uint32_t)newCanvasEntity);
        }
    }));
    
    eventSystem.registerAction("close_active_canvas", std::function<void()>([this]() {
        spdlog::info("Close active canvas action triggered");
        if (getCanvasManager() && getCanvasManager()->getCanvasCount() > 1) {
            size_t activeIndex = getCanvasManager()->getActiveCanvasIndex();
            getCanvasManager()->removeCanvas(activeIndex);
            spdlog::info("Closed canvas at index: {}", activeIndex);
        }
    }));
    
    eventSystem.registerAction("switch_canvas", std::function<void(size_t)>([this](size_t canvasIndex) {
        spdlog::info("Switch canvas action triggered: {}", canvasIndex);
        if (getCanvasManager()) {
            getCanvasManager()->setActiveCanvas(canvasIndex);
        }
    }));
    
    // Window management actions
    eventSystem.registerAction("get_all_windows", std::function<std::vector<std::string>()>([this]() -> std::vector<std::string> {
        if (getUIManager()) {
            return getUIManager()->getAllWindowNames();
        }
        return {};
    }));
    
    eventSystem.registerAction("get_window_visibility", std::function<bool(const std::string&)>([this](const std::string& windowName) -> bool {
        if (getUIManager()) {
            return getUIManager()->getWindowVisibility(windowName);
        }
        return false;
    }));
    
    eventSystem.registerAction("set_window_visibility", std::function<void(std::pair<std::string, bool>)>([this](std::pair<std::string, bool> data) {
        if (getUIManager()) {
            getUIManager()->setWindowVisibility(data.first, data.second);
        }
    }));
    
    // Workspace management actions
    eventSystem.registerAction("get_available_workspaces", std::function<std::vector<std::string>()>([this]() -> std::vector<std::string> {
        if (getUIManager()) {
            return getUIManager()->getAllWorkspaceNames();
        }
        return {};
    }));
    spdlog::info("[BlotApp] Registered get_available_workspaces: {}", eventSystem.hasAction("get_available_workspaces"));

    spdlog::info("[EventSystem] Registering load_workspace action");
    eventSystem.registerAction("load_workspace", std::function<void(const std::string&)>([this](const std::string& workspaceName) {
        if (getUIManager()) {
            getUIManager()->loadWorkspace(workspaceName);
        }
    }));
    spdlog::info("[BlotApp] Registered load_workspace: {}", eventSystem.hasAction("load_workspace"));
    
    eventSystem.registerAction("save_workspace", std::function<void(const std::string&)>([this](const std::string& workspaceName) {
        if (getUIManager()) {
            getUIManager()->saveWorkspace(workspaceName);
        }
    }));
    
    eventSystem.registerAction("get_current_workspace", std::function<std::string()>([this]() -> std::string {
        if (getUIManager()) {
            return getUIManager()->getCurrentWorkspace();
        }
        return "current";
    }));
    
    eventSystem.registerAction("get_debug_mode", std::function<bool()>([this]() -> bool {
        return blot::BlotEngine::getDebugMode();
    }));
}

void SampleUiApp::update() {
    // Update application logic
    getECSManager()->runCanvasSystems(getRenderingManager(), m_deltaTime);
    if (auto scriptEngine = getScriptEngine()) {
        scriptEngine->updateScript(m_deltaTime);
    }
    
    // Update ECS systems
    getECSManager()->updateSystems(m_deltaTime);
    
    // Update window manager
    if (getUIManager()) {
        getUIManager()->getWindowManager()->update();
    }
} 

// Implement new getter methods
blot::ECSManager* SampleUiApp::getECSManager() { return blot::BlotEngine::getECSManager(); }
bxScriptEngine* SampleUiApp::getScriptEngine() {
    auto ptr = getAddonManager()->getAddon("bxScriptEngine");
    return ptr ? dynamic_cast<bxScriptEngine*>(ptr.get()) : nullptr;
}
blot::RenderingManager* SampleUiApp::getRenderingManager() { return blot::BlotEngine::getRenderingManager(); }
blot::CanvasManager* SampleUiApp::getCanvasManager() { return blot::BlotEngine::getCanvasManager(); }
blot::UIManager* SampleUiApp::getUIManager() { return blot::BlotEngine::getUIManager(); }
SettingsManager& SampleUiApp::getSettings() { return blot::BlotEngine::getSettings(); }
const SettingsManager& SampleUiApp::getSettings() const { return blot::BlotEngine::getSettings(); }
bxCodeEditor* SampleUiApp::getCodeEditor() {
    auto ptr = getAddonManager()->getAddon("bxCodeEditor");
    return ptr ? dynamic_cast<bxCodeEditor*>(ptr.get()) : nullptr;
} 

void SampleUiApp::configureWindow(WindowSettings& settings) {
    settings.width = 1280;
    settings.height = 720;
    settings.title = "Blot Sample UI";
    settings.fullscreen = false;
} 