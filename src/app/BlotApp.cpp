#define NOMINMAX

// GLAD must be included before GLFW
#if defined(_WIN32) || (defined(__linux__) && !defined(__arm__) && !defined(__aarch64__))
    #include <glad/gl.h>
    #define GLAD_LOAD_FN gladLoaderLoadGL
#elif defined(__arm__) || defined(__aarch64__)
    #include <glad/gles2.h>
    #define GLAD_LOAD_FN gladLoaderLoadGLES2
#else
    #error "Unknown platform for GLAD loader"
#endif

// Standard library includes
#include <iostream>
#include <chrono>
#include <unordered_map>
#include <fstream>
#include <string>

// Third-party includes
#include <nlohmann/json.hpp>
#include <imgui.h>
#include <GLFW/glfw3.h>

#ifdef _WIN32
#include <windows.h>
#endif

// Project includes
#include "app/BlotApp.h"
#include "canvas/Canvas.h"
#include "rendering/Graphics.h"
#include "ui/TextRenderer.h"
#include "ui/ImGuiRenderer.h"
#include "ui/UIManager.h"
#include "scripting/ScriptEngine.h"
#include "addons/AddonManager.h"
#include "ui/windows/NodeEditorWindow.h"
#include "ui/windows/CanvasWindow.h"
#include "ecs/ECSManager.h"
#include "ui/CodeEditor.h"
#include "canvas/CanvasManager.h"

BlotApp::BlotApp() 
    : m_window(nullptr)
    , m_windowWidth(1280)
    , m_windowHeight(720)
    , m_running(true)
    , m_deltaTime(0.0f)
    , m_lastFrameTime(0.0f)
{
    // Load application settings
    m_settings.loadSettings();
    
    // Sync colors with settings (now handled by ToolbarWindow)
    
    initWindow();
    initGraphics();
    initAddons();
    
    // Initialize UI management
    m_uiManager = std::make_unique<blot::UIManager>(m_window);
    m_uiManager->initImGui();
    
    // Disable debug mode by default
    m_debugMode = false;
    m_uiManager->setDebugMode(false);
    
    // Set up window callbacks
    m_uiManager->setupWindowCallbacks(this);
    
    // Initialize code editor
    m_codeEditor = std::make_unique<CodeEditor>();
    if (m_codeEditor) {
        m_codeEditor->loadDefaultTemplate();
    }
}

BlotApp::~BlotApp() {
    // Save current ImGui layout
    if (m_uiManager) {
        m_uiManager->saveCurrentImGuiLayout();
    }
    
    // Save application settings
    m_settings.saveSettings();
    
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
    glfwTerminate();
}

void BlotApp::initWindow() {
    if (m_debugMode) {
        std::cout << "Initializing GLFW..." << std::endl;
    }
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    if (m_debugMode) {
        std::cout << "Creating window with size " << m_windowWidth << "x" << m_windowHeight << std::endl;
    }
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Blot - Creative Coding", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    if (m_debugMode) {
        std::cout << "Window created successfully" << std::endl;
    }
    glfwMakeContextCurrent(m_window);
    
    // Position the window in the center of the screen
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (monitor) {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        if (mode) {
            int x = (mode->width - m_windowWidth) / 2;
            int y = (mode->height - m_windowHeight) / 2;
            glfwSetWindowPos(m_window, x, y);
            if (m_debugMode) {
                std::cout << "Window positioned at " << x << "," << y << " on monitor " << mode->width << "x" << mode->height << std::endl;
            }
        }
    }
    
    // Initialize GLAD (GL or GLES2) after context is current
    if (!GLAD_LOAD_FN()) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    glfwSwapInterval(1); // Enable vsync
    
    // Set up callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto app = static_cast<BlotApp*>(glfwGetWindowUserPointer(window));
        app->m_windowWidth = width;
        app->m_windowHeight = height;
        glViewport(0, 0, width, height);
    });
    
    // Show the window
    glfwShowWindow(m_window);

    // Check for any GLFW errors
    const char* error;
    if (glfwGetError(&error) && error) {
        std::cout << "GLFW error: " << error << std::endl;
    }
}

void BlotApp::initGraphics() {
    m_graphics = std::make_shared<Graphics>();
    m_scriptEngine = std::make_unique<ScriptEngine>();
    m_ecsManager = std::make_unique<ECSManager>();
    
    // Initialize resource manager
    m_renderingManager = std::make_unique<RenderingManager>();
    
    // Initialize canvas manager
    m_canvasManager = std::make_unique<blot::CanvasManager>();
    
    // Create a default canvas
    auto defaultCanvas = m_canvasManager->createCanvas(m_windowWidth, m_windowHeight, "Default Canvas");
    
    // Create a default canvas as an ECS entity and resources
    m_activeCanvasId = m_ecsManager->createEntity();
    TextureComponent texComp;
    texComp.width = m_windowWidth;
    texComp.height = m_windowHeight;
    texComp.renderTarget = true;
    // texComp.textureId will be set by resource manager
    m_ecsManager->addComponent<TextureComponent>(m_activeCanvasId, texComp);
    (void)texComp; // Suppress unused variable warning
    
    // Create renderer through resource manager (returns shared_ptr)
    auto renderer = m_renderingManager->createRenderer(m_activeCanvasId, m_windowWidth, m_windowHeight);
    if (!renderer) {
        throw std::runtime_error("Failed to create renderer for canvas");
    }
    
    auto graphics = std::make_shared<Graphics>();
    graphics->setRenderer(renderer.get());
    auto canvas = std::make_unique<Canvas>(m_windowWidth, m_windowHeight, graphics);
    canvas->setECSManager(m_ecsManager.get()); // Connect Canvas to ECS
    m_renderingManager->addGraphics(m_activeCanvasId, graphics);
    m_renderingManager->addCanvas(m_activeCanvasId, std::move(canvas));
    

    
    // Final setup phase
    setup();
}

void BlotApp::setup() {
    // Final setup phase - everything is now initialized
    std::cout << "Setting up application..." << std::endl;
    
    // Connect ECS event system to UI components
    if (m_uiManager && m_ecsManager) {
        connectEventSystemToUI();
    }
    
    // Setup addons with access to all systems
    if (m_addonManager) {
        m_addonManager->setupAll();
    }
    
    // Run the default sketch on launch
    if (m_codeEditor) {
        m_scriptEngine->runCode(m_codeEditor->getCode());
    }
    
    std::cout << "Application setup complete" << std::endl;
}

void BlotApp::connectEventSystemToUI() {
    // Get the ECS event system
    auto& eventSystem = m_ecsManager->getEventSystem();
    
    // Connect MainMenuBar to the ECS event system
    auto mainMenuBar = std::dynamic_pointer_cast<blot::MainMenuBar>(
        m_uiManager->getWindowManager()->getWindow("MainMenuBar"));
    if (mainMenuBar) {
        mainMenuBar->setEventSystem(&eventSystem);
        
        // Connect MainMenuBar to CodeEditorWindow for theme state
        auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
            m_uiManager->getWindowManager()->getWindow("CodeEditor"));
        if (codeEditorWindow) {
            mainMenuBar->setCodeEditorWindow(codeEditorWindow);
        }
        
        // Connect MainMenuBar to CanvasManager for canvas operations
        if (m_canvasManager) {
            auto activeCanvas = m_canvasManager->getActiveCanvas();
            if (activeCanvas) {
                mainMenuBar->setCanvas(activeCanvas);
            }
            mainMenuBar->setCanvasManager(m_canvasManager.get());
        }
    }
    
    // Connect MainMenuBar to UIManager for ImGui theme
    if (mainMenuBar && m_uiManager) {
        mainMenuBar->setUIManager(m_uiManager.get());
    }
    
    // Connect NodeEditorWindow to the ECS system
    auto nodeEditorWindow = std::dynamic_pointer_cast<blot::NodeEditorWindow>(
        m_uiManager->getWindowManager()->getWindow("NodeEditor"));
    if (nodeEditorWindow) {
        // Create a shared_ptr from the unique_ptr for the window
        auto ecsSharedPtr = std::shared_ptr<ECSManager>(m_ecsManager.get(), [](ECSManager*){});
        nodeEditorWindow->setECSManager(ecsSharedPtr);
    }
    
    // Connect PropertiesWindow to the ECS system
    auto propertiesWindow = std::dynamic_pointer_cast<blot::PropertiesWindow>(
        m_uiManager->getWindowManager()->getWindow("Properties"));
    if (propertiesWindow) {
        // Create a shared_ptr from the unique_ptr for the window
        auto ecsSharedPtr = std::shared_ptr<ECSManager>(m_ecsManager.get(), [](ECSManager*){});
        propertiesWindow->setECSManager(ecsSharedPtr);
    }
    
    // Connect CanvasWindow to the ECS system
    auto canvasWindow = std::dynamic_pointer_cast<blot::CanvasWindow>(
        m_uiManager->getWindowManager()->getWindow("Canvas"));
    if (canvasWindow) {
        canvasWindow->setECSManager(m_ecsManager.get());
        canvasWindow->setRenderingManager(m_renderingManager.get());
        canvasWindow->setActiveCanvasId(m_activeCanvasId);
    }
    
    // Connect AddonManagerWindow to the AddonManager
    auto addonManagerWindow = std::dynamic_pointer_cast<blot::AddonManagerWindow>(
        m_uiManager->getWindowManager()->getWindow("AddonManager"));
    if (addonManagerWindow && m_addonManager) {
        // Create a shared_ptr from the unique_ptr for the window
        auto addonManagerSharedPtr = std::shared_ptr<AddonManager>(m_addonManager.get(), [](AddonManager*){});
        addonManagerWindow->setAddonManager(addonManagerSharedPtr);
    }
    
    // Connect AddonManager to the ECS event system
    if (m_addonManager) {
        connectAddonManagerToEventSystem(eventSystem);
    }
    
    // Connect CodeEditorWindow to the theme system
    auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
        m_uiManager->getWindowManager()->getWindow("CodeEditor"));
    if (codeEditorWindow) {
        // Set the current theme state in CodeEditorWindow
        codeEditorWindow->setCurrentTheme(static_cast<int>(m_uiManager->m_currentTheme));
    }
    
    // Register UI actions with the ECS event system
    registerUIActions(eventSystem);
}

void BlotApp::connectAddonManagerToEventSystem(blot::systems::EventSystem& eventSystem) {
    // Register addon-related events with the ECS event system
    eventSystem.registerEvent("addon_loaded", [this](const blot::systems::Event& event) {
        std::cout << "Addon loaded event: " << event.actionId << std::endl;
        // Trigger addon manager's global event
        if (m_addonManager) {
            m_addonManager->triggerGlobalEvent("addon_loaded");
        }
    });
    
    eventSystem.registerEvent("addon_unloaded", [this](const blot::systems::Event& event) {
        std::cout << "Addon unloaded event: " << event.actionId << std::endl;
        if (m_addonManager) {
            m_addonManager->triggerGlobalEvent("addon_unloaded");
        }
    });
    
    eventSystem.registerEvent("addon_enabled", [this](const blot::systems::Event& event) {
        std::cout << "Addon enabled event: " << event.actionId << std::endl;
        if (m_addonManager) {
            m_addonManager->triggerGlobalEvent("addon_enabled");
        }
    });
    
    eventSystem.registerEvent("addon_disabled", [this](const blot::systems::Event& event) {
        std::cout << "Addon disabled event: " << event.actionId << std::endl;
        if (m_addonManager) {
            m_addonManager->triggerGlobalEvent("addon_disabled");
        }
    });
}

void BlotApp::registerUIActions(blot::systems::EventSystem& eventSystem) {
    // File menu actions
    eventSystem.registerAction("new_sketch", [this]() {
        std::cout << "New sketch action triggered" << std::endl;
        // TODO: Implement new sketch functionality
    });
    
    eventSystem.registerAction("open_sketch", [this]() {
        std::cout << "Open sketch action triggered" << std::endl;
        // TODO: Implement open sketch functionality
    });
    
    eventSystem.registerAction("save_sketch", [this]() {
        std::cout << "Save sketch action triggered" << std::endl;
        // TODO: Implement save sketch functionality
    });
    
    eventSystem.registerAction("quit", [this]() {
        std::cout << "Quit action triggered" << std::endl;
        m_running = false;
    });
    
    // Edit menu actions
    eventSystem.registerAction("addon_manager", [this]() {
        std::cout << "Addon manager action triggered" << std::endl;
        auto addonManagerWindow = m_uiManager->getWindowManager()->getWindow("AddonManager");
        if (addonManagerWindow) {
            addonManagerWindow->show();
        }
    });
    
    eventSystem.registerAction("reload_addons", [this]() {
        std::cout << "Reload addons action triggered" << std::endl;
        if (m_addonManager) {
            m_addonManager->reloadAllAddons();
        }
    });
    
    // View menu actions
    eventSystem.registerAction("theme_editor", [this]() {
        std::cout << "Theme editor action triggered" << std::endl;
        auto themeEditorWindow = m_uiManager->getWindowManager()->getWindow("ThemeEditor");
        if (themeEditorWindow) {
            themeEditorWindow->show();
        }
    });
    
    eventSystem.registerAction("implot_demo", [this]() {
        std::cout << "ImPlot demo action triggered" << std::endl;
        // TODO: Implement ImPlot demo
    });
    
    eventSystem.registerAction("imgui_markdown_demo", [this]() {
        std::cout << "ImGui markdown demo action triggered" << std::endl;
        // TODO: Implement markdown demo
    });
    
    eventSystem.registerAction("markdown_editor", [this]() {
        std::cout << "Markdown editor action triggered" << std::endl;
        // TODO: Implement markdown editor
    });
    
    // Canvas menu actions
    eventSystem.registerAction("new_canvas", [this]() {
        std::cout << "New canvas action triggered" << std::endl;
        // TODO: Implement new canvas functionality
    });
    
    eventSystem.registerAction("save_canvas", [this]() {
        std::cout << "Save canvas action triggered" << std::endl;
        // TODO: Implement save canvas functionality
    });
    
    // Workspace menu actions
    eventSystem.registerAction("save_current_workspace", [this]() {
        std::cout << "Save current workspace action triggered" << std::endl;
        if (m_uiManager) {
            m_uiManager->saveWorkspace("current");
        }
    });
    
    eventSystem.registerAction("show_save_workspace_dialog", [this]() {
        std::cout << "Show save workspace dialog action triggered" << std::endl;
        auto saveDialog = m_uiManager->getWindowManager()->getWindow("SaveWorkspaceDialog");
        if (saveDialog) {
            saveDialog->show();
        }
    });
    
    // Run menu actions
    eventSystem.registerAction("run_sketch", [this]() {
        std::cout << "Run sketch action triggered" << std::endl;
        if (m_codeEditor) {
            m_scriptEngine->runCode(m_codeEditor->getCode());
        }
    });
    
    eventSystem.registerAction("stop_sketch", [this]() {
        std::cout << "Stop sketch action triggered" << std::endl;
        // TODO: Implement stop sketch functionality
    });
    
    // Theme switching actions
    eventSystem.registerAction("switch_theme", std::function<void(int)>([this](int theme) {
        std::cout << "Switch theme action triggered: " << theme << std::endl;
        if (m_uiManager) {
            m_uiManager->setImGuiTheme(static_cast<blot::UIManager::ImGuiTheme>(theme));
            
            // Update CodeEditorWindow theme state
            auto codeEditorWindow = std::dynamic_pointer_cast<blot::CodeEditorWindow>(
                m_uiManager->getWindowManager()->getWindow("CodeEditor"));
            if (codeEditorWindow) {
                codeEditorWindow->setCurrentTheme(theme);
            }
        }
    }));
    
    // Renderer switching actions
    eventSystem.registerAction("switch_renderer", std::function<void(int)>([this](int rendererType) {
        std::cout << "Switch renderer action triggered: " << rendererType << std::endl;
        // Switch renderer for the active canvas
        if (m_renderingManager) {
            auto canvasPtr = m_renderingManager->getCanvas(m_activeCanvasId);
            if (canvasPtr && *canvasPtr) {
                (*canvasPtr)->switchRenderer(static_cast<RendererType>(rendererType));
            }
        }
    }));
    
    // Debug actions
    eventSystem.registerAction("set_debug_mode", std::function<void(bool)>([this](bool enabled) {
        std::cout << "Set debug mode action triggered: " << enabled << std::endl;
        setDebugMode(enabled);
        if (m_uiManager) {
            m_uiManager->setDebugMode(enabled);
        }
    }));
    
    // Canvas management actions
    eventSystem.registerAction("select_canvas", std::function<void(uint32_t)>([this](uint32_t canvasId) {
        std::cout << "Select canvas action triggered: " << canvasId << std::endl;
        // TODO: Implement canvas selection
    }));
    
    eventSystem.registerAction("close_canvas", std::function<void(uint32_t)>([this](uint32_t canvasId) {
        std::cout << "Close canvas action triggered: " << canvasId << std::endl;
        // TODO: Implement canvas closing
    }));
    
    // Canvas management actions using CanvasManager
    eventSystem.registerAction("new_canvas", std::function<void()>([this]() {
        std::cout << "New canvas action triggered" << std::endl;
        if (m_canvasManager) {
            auto newCanvas = m_canvasManager->createCanvas(m_windowWidth, m_windowHeight);
            std::cout << "Created new canvas: " << newCanvas->getName() << std::endl;
        }
    }));
    
    eventSystem.registerAction("close_active_canvas", std::function<void()>([this]() {
        std::cout << "Close active canvas action triggered" << std::endl;
        if (m_canvasManager && m_canvasManager->getCanvasCount() > 1) {
            size_t activeIndex = m_canvasManager->getActiveCanvasIndex();
            m_canvasManager->removeCanvas(activeIndex);
            std::cout << "Closed canvas at index: " << activeIndex << std::endl;
        }
    }));
    
    eventSystem.registerAction("switch_canvas", std::function<void(size_t)>([this](size_t canvasIndex) {
        std::cout << "Switch canvas action triggered: " << canvasIndex << std::endl;
        if (m_canvasManager) {
            m_canvasManager->setActiveCanvas(canvasIndex);
        }
    }));
    
    // Window management actions
    eventSystem.registerAction("get_all_windows", std::function<std::vector<std::string>()>([this]() -> std::vector<std::string> {
        if (m_uiManager) {
            return m_uiManager->getAllWindowNames();
        }
        return {};
    }));
    
    eventSystem.registerAction("get_window_visibility", std::function<bool(const std::string&)>([this](const std::string& windowName) -> bool {
        if (m_uiManager) {
            return m_uiManager->getWindowVisibility(windowName);
        }
        return false;
    }));
    
    eventSystem.registerAction("set_window_visibility", std::function<void(const std::string&, bool)>([this](const std::string& windowName, bool visible) {
        if (m_uiManager) {
            m_uiManager->setWindowVisibility(windowName, visible);
        }
    }));
    
    // Workspace management actions
    eventSystem.registerAction("get_available_workspaces", std::function<std::vector<std::string>()>([this]() -> std::vector<std::string> {
        if (m_uiManager) {
            return m_uiManager->getAllWorkspaceNames();
        }
        return {};
    }));
    
    eventSystem.registerAction("load_workspace", std::function<void(const std::string&)>([this](const std::string& workspaceName) {
        if (m_uiManager) {
            m_uiManager->loadWorkspace(workspaceName);
        }
    }));
    
    eventSystem.registerAction("save_workspace", std::function<void(const std::string&)>([this](const std::string& workspaceName) {
        if (m_uiManager) {
            m_uiManager->saveWorkspace(workspaceName);
        }
    }));
    
    eventSystem.registerAction("get_current_workspace", std::function<std::string()>([this]() -> std::string {
        if (m_uiManager) {
            return m_uiManager->getCurrentWorkspace();
        }
        return "current";
    }));
    
    eventSystem.registerAction("get_debug_mode", std::function<bool()>([this]() -> bool {
        return getDebugMode();
    }));
}

void BlotApp::initAddons() {
    m_addonManager = std::make_unique<AddonManager>();
    
    // Initialize default addons through AddonManager
    m_addonManager->initDefaultAddons();
}

void BlotApp::run() {
    if (!m_window) {
        std::cout << "Error: Window is null! Exiting run()." << std::endl;
        return;
    }

    std::cout << "Entering main loop..." << std::endl;
    while (!glfwWindowShouldClose(m_window) && m_running) {
        float currentTime = static_cast<float>(glfwGetTime());
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        
        glfwPollEvents();
        
        // Update application logic
        update();

        // Clear and render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Check for OpenGL errors
        if (m_debugMode) {
            GLenum err;
            while ((err = glGetError()) != GL_NO_ERROR) {
                std::cout << "OpenGL error: " << err << std::endl;
            }
        }
        
        // Update UI through UIManager
        m_uiManager->update();
        
        glfwSwapBuffers(m_window);
    }
    std::cout << "Exited main loop." << std::endl;
} 

void BlotApp::update() {
    // Update application logic
    m_ecsManager->runCanvasSystems(m_renderingManager.get(), m_deltaTime);
    m_scriptEngine->update(m_deltaTime);
    if (m_addonManager) {
        m_addonManager->updateAll(m_deltaTime);
    }
    
    // Update ECS systems
    m_ecsManager->updateSystems(m_deltaTime);
    
    // Update window manager
    if (m_uiManager) {
        m_uiManager->getWindowManager()->update();
    }
} 