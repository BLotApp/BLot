#define NOMINMAX
#if defined(_WIN32) || (defined(__linux__) && !defined(__arm__) && !defined(__aarch64__))
    #include <glad/gl.h>
    #define GLAD_LOAD_FN gladLoaderLoadGL
#elif defined(__arm__) || defined(__aarch64__)
    #include <glad/gles2.h>
    #define GLAD_LOAD_FN gladLoaderLoadGLES2
#else
    #error "Unknown platform for GLAD loader"
#endif

#include <iostream>
#include <chrono>
#include "app/BlotApp.h"
#include "canvas/Canvas.h"
#include "rendering/Graphics.h"
#include "ui/TextRenderer.h"
#include "ui/ImGuiRenderer.h"
#include "ui/UIManager.h"
// #include "codeeditor/CodeEditor.h" // TODO: Implement CodeEditor class
#include "scripting/ScriptEngine.h"
#include "addons/AddonManager.h"
#include "ui/windows/NodeEditorWindow.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include <imgui_node_editor.h>
namespace ed = ax::NodeEditor;

#include <unordered_map>
#include <nlohmann/json.hpp>
#include <fstream>
#include <string>

// Font is included in UIManager.cpp
#include "rendering/Blend2DRenderer.h"
#include "../third_party/portable-file-dialogs/portable-file-dialogs.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "ecs/ECSManager.h"
#include "ecs/components/TextureComponent.h"
#include "ecs/systems/CanvasUpdateSystem.h"
#include "ecs/systems/ShapeRenderingSystem.h"

#include "implot.h"
#include "implot3d.h"

#include "imgui_markdown.h"
#include "imfilebrowser.h"

#include <vector>

#include "ui/windows/StrokeWindow.h"
#include "ui/windows/SaveWorkspaceDialog.h"
#include "ui/CodeEditor.h"

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 operator*(const ImVec2& a, float s) { return ImVec2(a.x * s, a.y * s); }
inline ImVec2 operator/(const ImVec2& a, float s) { return ImVec2(a.x / s, a.y / s); }

// Add a member variable to control ImPlot demo window visibility
static bool showImGuiMarkdownDemo = false;

// Mouse coordinate display

static int mouseCoordinateSystem = 0; // 0=Screen, 1=App, 2=Window, 3=Canvas
static const char* coordinateSystemNames[] = { "Screen", "App", "Window", "Canvas" };
static bool showMarkdownEditor = false;
static bool showMarkdownViewer = false;
static std::string markdownEditorBuffer;
static std::string loadedMarkdownPath;
static std::string loadedMarkdown;

bool showFileBrowser = false;
int m_selectedShape = 0; // 0=Rectangle, 1=Ellipse, 2=Line, 3=Polygon, 4=Star
float m_dashPattern[4] = {0};
int m_dashCount = 0;
float m_dashOffset = 0.0f;

// Add polygon/star sides control
static int m_polygonSides = 5;



// Replace AddEllipse and AddEllipseFilled usages with custom ellipse drawing
auto DrawEllipseFilled = [](ImDrawList* drawList, ImVec2 center, float rx, float ry, ImU32 color, int num_segments = 64) {
    std::vector<ImVec2> pts;
    for (int i = 0; i < num_segments; ++i) {
        float a = 2.0f * 3.1415926f * i / num_segments;
        pts.push_back(center + ImVec2(cosf(a) * rx, sinf(a) * ry));
    }
    drawList->AddConvexPolyFilled(pts.data(), static_cast<int>(pts.size()), color);
};
auto DrawEllipse = [](ImDrawList* drawList, ImVec2 center, float rx, float ry, ImU32 color, float thickness = 1.0f, int num_segments = 64) {
    std::vector<ImVec2> pts;
    for (int i = 0; i < num_segments; ++i) {
        float a = 2.0f * 3.1415926f * i / num_segments;
        pts.push_back(center + ImVec2(cosf(a) * rx, sinf(a) * ry));
    }
    drawList->AddPolyline(pts.data(), static_cast<int>(pts.size()), color, true, thickness);
};

BlotApp* g_blotAppInstance = nullptr;

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
    
    // Create and register texture viewer window
    auto textureWindow = std::make_shared<blot::TextureViewerWindow>("Texture###MainTexture", 
                                                          blot::Window::Flags::NoScrollbar | blot::Window::Flags::NoCollapse);
    m_uiManager->getWindowManager()->createWindow("Texture", textureWindow);
    
    // Create toolbar window
    auto toolbarWindow = std::make_shared<blot::ToolbarWindow>("Toolbar###MainToolbar",
        blot::Window::Flags::None);
    m_uiManager->getWindowManager()->createWindow("Toolbar", toolbarWindow);
    blot::WindowSettingsComponent toolbarSettings;
    toolbarSettings.category = "Tools";
    toolbarSettings.showByDefault = true;
    toolbarSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("Toolbar", toolbarSettings);
    
    // Configure toolbar callbacks
    toolbarWindow->setOnStrokeWidthChanged([this](float width) {
        // Get stroke window through WindowManager
        auto strokeWindow = std::dynamic_pointer_cast<StrokeWindow>(
            m_uiManager->getWindowManager()->getWindow("Stroke"));
        if (strokeWindow) {
            strokeWindow->setStrokeWidth(static_cast<double>(width));
        }
    });
    
    auto infoWindow = std::make_shared<blot::InfoWindow>("Info Window###MainInfoWindow", 
                                                          blot::Window::Flags::AlwaysAutoResize);
    m_uiManager->getWindowManager()->createWindow("InfoWindow", infoWindow);
    
    auto propertiesWindow = std::make_shared<blot::PropertiesWindow>("Properties###MainProperties", 
                                                            blot::Window::Flags::None);
    m_uiManager->getWindowManager()->createWindow("Properties", propertiesWindow);
    
    // Configure window settings
    blot::WindowSettingsComponent infoPanelSettings;
    infoPanelSettings.category = "Debug";
    infoPanelSettings.showByDefault = true;
    infoPanelSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("InfoPanel", infoPanelSettings);
    
    blot::WindowSettingsComponent propertiesSettings;
    propertiesSettings.category = "Tools";
    propertiesSettings.showByDefault = false;
    propertiesSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("Properties", propertiesSettings);
    
    blot::WindowSettingsComponent canvasSettings;
    canvasSettings.category = "Main";
    canvasSettings.showByDefault = true;
    canvasSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("Canvas", canvasSettings);
    
    // Create and register code editor window
    auto codeEditorWindow = std::make_shared<blot::CodeEditorWindow>("Code Editor###MainCodeEditor", 
                                                           blot::Window::Flags::None);
    m_uiManager->getWindowManager()->createWindow("CodeEditor", codeEditorWindow);
    
    // Configure code editor window settings
    blot::WindowSettingsComponent codeEditorSettings;
    codeEditorSettings.category = "Tools";
    codeEditorSettings.showByDefault = true;
    codeEditorSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("CodeEditor", codeEditorSettings);
    
    // Set the code editor in the window
    // Note: CodeEditorWindow manages its own TextEditor internally
    
    // Create and register main menu bar
    auto mainMenuBar = std::make_shared<blot::MainMenuBar>("MainMenuBar###MainMenuBar", 
                                                   blot::Window::Flags::None);
    m_uiManager->getWindowManager()->createWindow("MainMenuBar", mainMenuBar);
    
    // Configure main menu bar settings
    blot::WindowSettingsComponent menuBarSettings;
    menuBarSettings.category = "Main";
    menuBarSettings.showByDefault = true;
    menuBarSettings.showInMenu = false; // Don't show in Windows menu since it's always visible
    m_uiManager->getWindowManager()->setWindowSettings("MainMenuBar", menuBarSettings);
    
    // Configure main menu bar callbacks
    mainMenuBar->setQuitCallback([this]() {
        glfwSetWindowShouldClose(m_window, true);
    });
    
    mainMenuBar->setNewSketchCallback([this]() {
        // TODO: Implement new sketch functionality
    });
    
    mainMenuBar->setOpenSketchCallback([this]() {
        // TODO: Implement open sketch functionality
    });
    
    mainMenuBar->setSaveSketchCallback([this]() {
        // TODO: Implement save sketch functionality
    });
    
    mainMenuBar->setRunSketchCallback([this]() {
        // TODO: Implement run sketch functionality
    });
    
    mainMenuBar->setStopSketchCallback([this]() {
        // TODO: Implement stop sketch functionality
    });
    
    mainMenuBar->setAddonManagerCallback([this]() {
        auto addonManagerWindow = m_uiManager->getWindowManager()->getWindow("AddonManager");
        if (addonManagerWindow) {
            addonManagerWindow->show();
        }
    });
    
    mainMenuBar->setReloadAddonsCallback([this]() {
        if (m_addonManager) {
            m_addonManager->reloadAllAddons();
        }
    });
    
    mainMenuBar->setThemeEditorCallback([this]() {
        auto themeEditorWindow = m_uiManager->getWindowManager()->getWindow("ThemeEditor");
        if (themeEditorWindow) {
            themeEditorWindow->show();
        }
    });
    
    mainMenuBar->setNewCanvasCallback([this]() {
        // TODO: Implement new canvas functionality
    });
    
    mainMenuBar->setSaveCanvasCallback([this]() {
        // TODO: Implement save canvas functionality
    });
    
    mainMenuBar->setOpenMarkdownCallback([this]() {
        // TODO: Implement open markdown functionality
    });
    
    mainMenuBar->setSaveMarkdownCallback([this]() {
        // TODO: Implement save markdown functionality
    });
    
    mainMenuBar->setImPlotDemoCallback([this]() {
        // TODO: Implement ImPlot demo functionality
    });
    
    mainMenuBar->setImGuiMarkdownDemoCallback([this]() {
        // TODO: Implement ImGui Markdown demo functionality
    });
    
    mainMenuBar->setMarkdownEditorCallback([this]() {
        // TODO: Implement markdown editor functionality
    });
    
    mainMenuBar->setSwitchRendererCallback([this](int type) {
        // TODO: Implement renderer switching
    });
    
    mainMenuBar->setSwitchThemeCallback([this](int theme) {
        // TODO: Implement theme switching
    });
    
    // Window visibility management callbacks
    mainMenuBar->setWindowVisibilityCallback([this](const std::string& windowName, bool visible) {
        m_uiManager->setWindowVisibility(windowName, visible);
    });
    
    mainMenuBar->setGetWindowVisibilityCallback([this](const std::string& windowName) {
        return m_uiManager->getWindowVisibility(windowName);
    });
    
    mainMenuBar->setGetAllWindowsCallback([this]() {
        return m_uiManager->getAllWindowNames();
    });
    
    // Workspace management callbacks
    mainMenuBar->setLoadWorkspaceCallback([this](const std::string& workspaceName) {
        m_uiManager->loadWorkspace(workspaceName);
    });
    
    mainMenuBar->setSaveWorkspaceCallback([this](const std::string& workspaceName) {
        m_uiManager->saveWorkspace(workspaceName);
    });
    
    mainMenuBar->setSaveWorkspaceAsCallback([this](const std::string& workspaceName) {
        m_uiManager->saveWorkspaceAs(workspaceName);
    });
    
    mainMenuBar->setCurrentWorkspaceCallback([this]() {
        return m_uiManager->getCurrentWorkspace();
    });
    
    mainMenuBar->setGetAvailableWorkspacesCallback([this]() {
        return m_uiManager->getAvailableWorkspaces();
    });
    
    mainMenuBar->setShowSaveWorkspaceDialogCallback([this]() {
        auto saveDialog = m_uiManager->getSaveWorkspaceDialog();
        if (saveDialog) {
            saveDialog->setInitialWorkspaceName(m_uiManager->getCurrentWorkspace());
            saveDialog->setSaveCallback([this](const std::string& workspaceName) {
                m_uiManager->saveWorkspaceAs(workspaceName);
            });
            saveDialog->setCancelCallback([this]() {
                // Dialog will close itself
            });
            saveDialog->show();
        }
    });
    
    // Debug mode callbacks
    mainMenuBar->setDebugModeCallback([this](bool enabled) {
        m_debugMode = enabled;
        m_uiManager->setDebugMode(enabled);
    });
    
    mainMenuBar->setGetDebugModeCallback([this]() {
        return m_debugMode;
    });

    // Create and register Addon Manager window
    auto addonManagerWindow = std::make_shared<blot::AddonManagerWindow>("Addon Manager###AddonManager", blot::Window::Flags::None);
    addonManagerWindow->setAddonManager(std::shared_ptr<AddonManager>(m_addonManager.get(), [](AddonManager*){})); // non-owning
    m_uiManager->getWindowManager()->createWindow("AddonManager", addonManagerWindow);
    blot::WindowSettingsComponent addonManagerSettings;
    addonManagerSettings.category = "Tools";
    addonManagerSettings.showByDefault = false;
    addonManagerSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("AddonManager", addonManagerSettings);

    // Create and register Node Editor window
    auto nodeEditorWindow = std::make_shared<blot::NodeEditorWindow>("Node Editor###NodeEditor", blot::Window::Flags::None);
    nodeEditorWindow->setECSManager(std::shared_ptr<ECSManager>(m_ecsManager.get(), [](ECSManager*){}));
    m_uiManager->getWindowManager()->createWindow("NodeEditor", nodeEditorWindow);
    blot::WindowSettingsComponent nodeEditorSettings;
    nodeEditorSettings.category = "Tools";
    nodeEditorSettings.showByDefault = false;
    nodeEditorSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("NodeEditor", nodeEditorSettings);

    // Create and register Theme Editor window
    auto themeEditorWindow = std::make_shared<blot::ThemeEditorWindow>("Theme Editor###ThemeEditor", blot::Window::Flags::None);
    themeEditorWindow->setUIManager(m_uiManager.get());
    m_uiManager->getWindowManager()->createWindow("ThemeEditor", themeEditorWindow);
    blot::WindowSettingsComponent themeEditorSettings;
    themeEditorSettings.category = "Tools";
    themeEditorSettings.showByDefault = false;
    themeEditorSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("ThemeEditor", themeEditorSettings);

    // Create stroke window
    auto strokeWindow = std::make_shared<StrokeWindow>();
    strokeWindow->setStrokeWidthCallback([this](double width) {
        // Get toolbar window through WindowManager
        auto toolbarWindow = std::dynamic_pointer_cast<blot::ToolbarWindow>(
            m_uiManager->getWindowManager()->getWindow("Toolbar"));
        if (toolbarWindow) {
            toolbarWindow->setStrokeWidth(static_cast<float>(width));
        }
    });
    
    strokeWindow->setStrokeCapCallback([this](BLStrokeCap cap) {
        // Get the active canvas and its renderer
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeCap(cap);
                }
            }
        }
    });
    
    strokeWindow->setStrokeJoinCallback([this](BLStrokeJoin join) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeJoin(join);
                }
            }
        }
    });
    
    strokeWindow->setMiterLimitCallback([this](double limit) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeMiterLimit(limit);
                }
            }
        }
    });
    
    strokeWindow->setDashArrayCallback([this](const std::vector<double>& dashes) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeDashArray(dashes);
                }
            }
        }
    });
    
    strokeWindow->setDashOffsetCallback([this](double offset) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeDashOffset(offset);
                }
            }
        }
    });
    
    strokeWindow->setTransformOrderCallback([this](BLStrokeTransformOrder order) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            auto graphics = (*canvasPtr)->getGraphics();
            if (graphics) {
                auto renderer = graphics->getRenderer();
                if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(renderer)) {
                    blend2dRenderer->setStrokeTransformOrder(order);
                }
            }
        }
    });
    
    // Create a wrapper window for the StrokeWindow
    auto strokeWindowWrapper = std::make_shared<blot::ImGuiWindow>("Stroke###StrokeWindow", blot::Window::Flags::None);
    strokeWindowWrapper->setRenderCallback([strokeWindow]() {
        strokeWindow->render();
    });
    
    m_uiManager->getWindowManager()->createWindow("Stroke", strokeWindowWrapper);
    blot::WindowSettingsComponent strokeSettings;
    strokeSettings.category = "Tools";
    strokeSettings.showByDefault = false;
    strokeSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("Stroke", strokeSettings);
}

BlotApp::~BlotApp() {
    // Save current ImGui layout
    if (m_uiManager) {
        m_uiManager->saveCurrentImGuiLayout();
    }
    
    // Save application settings
    m_settings.saveSettings();
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
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
    if (m_debugMode) {
        std::cout << "Initializing GLAD..." << std::endl;
    }
    if (!GLAD_LOAD_FN()) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
        throw std::runtime_error("Failed to initialize GLAD");
    }
    
    if (m_debugMode) {
        std::cout << "GLAD initialized successfully" << std::endl;
    }
    glfwSwapInterval(1); // Enable vsync
    
    // Set up callbacks
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height) {
        auto app = static_cast<BlotApp*>(glfwGetWindowUserPointer(window));
        app->m_windowWidth = width;
        app->m_windowHeight = height;
        glViewport(0, 0, width, height);
        std::cout << "Window resized to " << width << "x" << height << std::endl;
    });
    
    // Show the window
    glfwShowWindow(m_window);
    if (m_debugMode) {
        std::cout << "Window should now be visible" << std::endl;
    }
    
    // Check if window is visible
    if (glfwGetWindowAttrib(m_window, GLFW_VISIBLE)) {
        if (m_debugMode) {
            std::cout << "Window is marked as visible" << std::endl;
        }
    } else {
        std::cout << "Warning: Window is not marked as visible!" << std::endl;
    }
    
    // Check for any GLFW errors
    const char* error;
    if (glfwGetError(&error) && error) {
        std::cout << "GLFW error: " << error << std::endl;
    }
}

void BlotApp::initGraphics() {
    m_graphics = std::make_shared<Graphics>();
    m_codeEditor = std::make_unique<CodeEditor>();
    m_scriptEngine = std::make_unique<ScriptEngine>();
    m_ecsManager = std::make_unique<ECSManager>();
    
    // Set up default creative coding environment
    m_codeEditor->loadDefaultTemplate();
    
    // Initialize resource manager
    m_resourceManager = std::make_unique<ResourceManager>();
    
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
    auto renderer = m_resourceManager->createRenderer(m_activeCanvasId, m_windowWidth, m_windowHeight);
    if (!renderer) {
        throw std::runtime_error("Failed to create renderer for canvas");
    }
    
    auto graphics = std::make_shared<Graphics>();
    graphics->setRenderer(renderer.get());
    auto canvas = std::make_unique<Canvas>(m_windowWidth, m_windowHeight, graphics);
    canvas->setECSManager(m_ecsManager.get()); // Connect Canvas to ECS
    m_resourceManager->addGraphics(m_activeCanvasId, graphics);
    m_resourceManager->addCanvas(m_activeCanvasId, std::move(canvas));
    
    // Initialize shape rendering system with the shared_ptr renderer
    m_shapeRenderer = std::make_unique<ShapeRenderingSystem>(renderer);
    
    // Final setup phase
    setup();
}

void BlotApp::setup() {
    // Final setup phase - everything is now initialized
    std::cout << "Setting up application..." << std::endl;
    
    // Setup addons with access to all systems
    if (m_addonManager) {
        m_addonManager->setupAll();
    }
    
    // Run the default sketch on launch
    m_scriptEngine->runCode(m_codeEditor->getCode());
    
    std::cout << "Application setup complete" << std::endl;
}

void BlotApp::initAddons() {
    m_addonManager = std::make_unique<AddonManager>();
    
    // Initialize default addons through AddonManager
    m_addonManager->initDefaultAddons();
}

void BlotApp::run() {
    g_blotAppInstance = this;
    if (m_debugMode) {
        std::cout << "Starting main loop..." << std::endl;
    }
    int frameCount = 0;
    
    // Check if window is still valid
    if (!m_window) {
        std::cout << "Error: Window is null! Exiting run()." << std::endl;
        return;
    }

    std::cout << "Entering main loop..." << std::endl;
    std::cout << "glfwWindowShouldClose(m_window): " << glfwWindowShouldClose(m_window) << std::endl;
    std::cout << "m_running: " << m_running << std::endl;
    std::cout << "About to enter while loop..." << std::endl;
    while (!glfwWindowShouldClose(m_window) && m_running) {
        frameCount++;
        std::cout << "Frame " << frameCount << std::endl;
        
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
        
        // Start ImGui frame
        std::cout << "ImGui_ImplOpenGL3_NewFrame()" << std::endl;
        ImGui_ImplOpenGL3_NewFrame();
        std::cout << "ImGui_ImplGlfw_NewFrame()" << std::endl;
        ImGui_ImplGlfw_NewFrame();
        std::cout << "ImGui::NewFrame()" << std::endl;
        ImGui::NewFrame();
        
        std::cout << "UIManager::update()" << std::endl;
        m_uiManager->update();
        
        std::cout << "ImGui::Render()" << std::endl;
        ImGui::Render();
        
        ImDrawData* drawData = ImGui::GetDrawData();
        if (drawData) {
            std::cout << "ImGui draw lists: " << drawData->CmdListsCount << ", total vtx: " << drawData->TotalVtxCount << std::endl;
        } else {
            std::cout << "ImGui draw data is null!" << std::endl;
        }
        
        std::cout << "ImGui_ImplOpenGL3_RenderDrawData()" << std::endl;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (m_debugMode && frameCount == 1) {
            std::cout << "First frame - ImGui rendered" << std::endl;
        }
        
        // Debug: Check if ImGui has any draw data
        if (m_debugMode) {
            ImDrawData* drawData = ImGui::GetDrawData();
            if (drawData && drawData->CmdListsCount > 0) {
                if (frameCount % 60 == 0) {
                    std::cout << "ImGui has " << drawData->CmdListsCount << " command lists" << std::endl;
                }
            } else {
                if (frameCount % 60 == 0) {
                    std::cout << "Warning: ImGui has no draw data!" << std::endl;
                }
            }
        }
        
        // Update and render additional viewports
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(m_window);
        
        // Check if window was closed
        if (m_debugMode && glfwWindowShouldClose(m_window)) {
            std::cout << "Window was requested to close after " << frameCount << " frames" << std::endl;
        }
    }
    std::cout << "Exited main loop after " << frameCount << " frames." << std::endl;
    if (m_debugMode) {
        std::cout << "Main loop ended after " << frameCount << " frames" << std::endl;
    }
} 

void BlotApp::update() {
    // Update application logic
    CanvasUpdateSystem(*m_ecsManager, m_resourceManager.get(), m_deltaTime);
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

// ECS-style system: update all canvases
void CanvasUpdateSystem(ECSManager& ecs, ResourceManager* resourceManager, float deltaTime) {
    auto view = ecs.view<TextureComponent>();
    for (auto entity : view) {
        auto canvasPtr = resourceManager->getCanvas(entity);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->update(deltaTime);
        }
    }
}

// ECS-style system: render all canvases (optionally, only active)
void CanvasRenderSystem(ECSManager& ecs, ResourceManager* resourceManager, entt::entity activeCanvasId) {
    if (activeCanvasId != entt::null && ecs.hasComponent<TextureComponent>(activeCanvasId)) {
        auto canvasPtr = resourceManager->getCanvas(activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->render();
        }
    }
} 