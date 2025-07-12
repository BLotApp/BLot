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
#include "codeeditor/CodeEditor.h"
#include "scripting/ScriptEngine.h"
#include "addons/AddonManager.h"
#include "node/NodeEditor.h"
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

#include "../assets/fonts/fontRobotoRegular.h"
#include "rendering/Blend2DRenderer.h"
#include "../third_party/portable-file-dialogs/portable-file-dialogs.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include "ecs/ECSManager.h"
#include "components/TextureComponent.h"
#include "systems/CanvasUpdateSystem.h"
#include "systems/ShapeRenderingSystem.h"

#include "implot.h"
#include "implot3d.h"

#include "imgui_markdown.h"
#include "imfilebrowser.h"

#include <vector>

#include "ui/StrokeWindow.h"

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 operator*(const ImVec2& a, float s) { return ImVec2(a.x * s, a.y * s); }
inline ImVec2 operator/(const ImVec2& a, float s) { return ImVec2(a.x / s, a.y / s); }

// Add a member variable to control ImPlot demo window visibility
static bool showImPlotDemo = false;
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
    , m_lastThemePath("theme.json")
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
    
    // Create and register texture viewer window
    auto textureWindow = std::make_shared<blot::TextureViewerWindow>("Texture###MainTexture", 
                                                          blot::Window::Flags::NoScrollbar | blot::Window::Flags::NoCollapse);
    m_uiManager->getWindowManager()->createWindow("Texture", textureWindow);
    
    // Create and register UI component windows
    m_toolbarWindow = std::make_shared<blot::ToolbarWindow>("Toolbar###MainToolbar", 
                                                      blot::Window::Flags::NoTitleBar | blot::Window::Flags::AlwaysAutoResize);
    m_uiManager->getWindowManager()->createWindow("Toolbar", m_toolbarWindow);
    
    auto infoWindow = std::make_shared<blot::InfoWindow>("Info Window###MainInfoWindow", 
                                                          blot::Window::Flags::AlwaysAutoResize);
    m_uiManager->getWindowManager()->createWindow("InfoWindow", infoWindow);
    
    auto propertiesWindow = std::make_shared<blot::PropertiesWindow>("Properties###MainProperties", 
                                                            blot::Window::Flags::None);
    m_uiManager->getWindowManager()->createWindow("Properties", propertiesWindow);
    
    // Configure window settings
    blot::WindowSettingsComponent toolbarSettings;
    toolbarSettings.category = "Tools";
    toolbarSettings.showByDefault = true;
    toolbarSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("Toolbar", toolbarSettings);
    
    // Connect toolbar stroke width changes to stroke window
    m_toolbarWindow->setOnStrokeWidthChanged([this](float width) {
        if (m_strokeWindow) {
            m_strokeWindow->setStrokeWidth(static_cast<double>(width));
        }
    });
    
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
    codeEditorWindow->setCodeEditor(std::shared_ptr<CodeEditor>(m_codeEditor.release()));
    
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
    configureMainMenuBarCallbacks();

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
    nodeEditorWindow->setNodes(&m_nodes);
    nodeEditorWindow->setNextNodeId(&m_nextNodeId);
    m_uiManager->getWindowManager()->createWindow("NodeEditor", nodeEditorWindow);
    blot::WindowSettingsComponent nodeEditorSettings;
    nodeEditorSettings.category = "Tools";
    nodeEditorSettings.showByDefault = false;
    nodeEditorSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("NodeEditor", nodeEditorSettings);

    // Create and register Theme Editor window
    auto themeEditorWindow = std::make_shared<blot::ThemeEditorWindow>("Theme Editor###ThemeEditor", blot::Window::Flags::None);
    themeEditorWindow->setThemePath(&m_lastThemePath);
    themeEditorWindow->setCurrentTheme(reinterpret_cast<int*>(&m_currentTheme));
    m_uiManager->getWindowManager()->createWindow("ThemeEditor", themeEditorWindow);
    blot::WindowSettingsComponent themeEditorSettings;
    themeEditorSettings.category = "Tools";
    themeEditorSettings.showByDefault = false;
    themeEditorSettings.showInMenu = true;
    m_uiManager->getWindowManager()->setWindowSettings("ThemeEditor", themeEditorSettings);

    // Create and register stroke window with comprehensive stroke options
    auto strokeWindow = std::make_shared<StrokeWindow>();
    m_strokeWindow = strokeWindow;
    
    // Connect stroke window callbacks to Blend2D renderer
    strokeWindow->setStrokeWidthCallback([this](double width) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeWidth(static_cast<float>(width));
        }
        // Sync with toolbar
        if (m_toolbarWindow) {
            m_toolbarWindow->setStrokeWidth(static_cast<float>(width));
        }
    });
    
    strokeWindow->setStrokeCapCallback([this](BLStrokeCap cap) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeCap(cap);
        }
    });
    
    strokeWindow->setStrokeJoinCallback([this](BLStrokeJoin join) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeJoin(join);
        }
    });
    
    strokeWindow->setMiterLimitCallback([this](double limit) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeMiterLimit(limit);
        }
    });
    
    strokeWindow->setDashArrayCallback([this](const std::vector<double>& dashes) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeDashArray(dashes);
        }
    });
    
    strokeWindow->setDashOffsetCallback([this](double offset) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeDashOffset(offset);
        }
    });
    
    strokeWindow->setTransformOrderCallback([this](BLStrokeTransformOrder order) {
        if (auto blend2dRenderer = dynamic_cast<Blend2DRenderer*>(m_currentRenderer.get())) {
            blend2dRenderer->setStrokeTransformOrder(order);
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
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    m_window = glfwCreateWindow(m_windowWidth, m_windowHeight, "Blot - Creative Coding", nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }
    
    glfwMakeContextCurrent(m_window);
    
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
}

void BlotApp::initGraphics() {
    m_graphics = std::make_shared<Graphics>();
    m_codeEditor = std::make_unique<CodeEditor>();
    m_scriptEngine = std::make_unique<ScriptEngine>();
    
    // Set up default creative coding environment
    m_codeEditor->loadDefaultTemplate();
    
    // Use only Blend2D renderer
    m_currentRendererType = RendererType::Blend2D;
    m_currentRenderer = std::make_unique<Blend2DRenderer>();
    m_currentRenderer->initialize(m_windowWidth, m_windowHeight);
    m_graphics->setRenderer(m_currentRenderer.get());
    
    // Initialize resource manager
    m_resourceManager = std::make_unique<ResourceManager>();
    
    // Create a default canvas as an ECS entity and resources
    m_activeCanvasId = m_ecs.createEntity();
    TextureComponent texComp;
    texComp.width = m_windowWidth;
    texComp.height = m_windowHeight;
    texComp.renderTarget = true;
    // texComp.textureId will be set by resource manager
    m_ecs.addComponent<TextureComponent>(m_activeCanvasId, texComp);
    (void)texComp; // Suppress unused variable warning
    
    // Create renderer through resource manager (returns shared_ptr)
    auto renderer = m_resourceManager->createRenderer(m_activeCanvasId, m_windowWidth, m_windowHeight);
    if (!renderer) {
        throw std::runtime_error("Failed to create renderer for canvas");
    }
    
    auto graphics = std::make_shared<Graphics>();
    graphics->setRenderer(renderer.get());
    auto canvas = std::make_unique<Canvas>(m_windowWidth, m_windowHeight, graphics);
    canvas->setECSManager(&m_ecs); // Connect Canvas to ECS
    m_resourceManager->addGraphics(m_activeCanvasId, graphics);
    m_resourceManager->addCanvas(m_activeCanvasId, std::move(canvas));
    
    // Initialize shape rendering system with the shared_ptr renderer
    m_shapeRenderer = std::make_unique<ShapeRenderingSystem>(renderer);
    
    // Run the default sketch on launch
    m_scriptEngine->runCode(m_codeEditor->getCode());
}

void BlotApp::initAddons() {
    m_addonManager = std::make_unique<AddonManager>();
    
    // Set up addon directory
    m_addonManager->setAddonDirectory("addons");
    m_addonManager->scanAddonDirectory("addons");
    
    // Load default addons
    loadDefaultAddons();
    
    // Initialize all addons
    if (!m_addonManager->initAll()) {
        std::cerr << "Failed to initialize addons" << std::endl;
    }
}

void BlotApp::loadDefaultAddons() {
    // In a real implementation, you would dynamically load addons
    // For now, we'll just register some example addons
    std::cout << "Loading default addons..." << std::endl;
    
    // Example: Register GUI addon
    // auto guiAddon = std::make_shared<bxGui>();
    // m_addonManager->registerAddon(guiAddon);
    
    // Example: Register OSC addon
    // auto oscAddon = std::make_shared<bxOsc>();
    // m_addonManager->registerAddon(oscAddon);
}

void BlotApp::run() {
    g_blotAppInstance = this;
    while (!glfwWindowShouldClose(m_window) && m_running) {
        float currentTime = static_cast<float>(glfwGetTime());
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        
        glfwPollEvents();
        handleInput();
        update();

        // Clear and render (move this to the start of the frame)
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        renderUI();
        renderCanvas();
        renderAddonUI();
        
        // Render ImGui
        ImGui::Render();
        
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        // Update and render additional viewports
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
        
        glfwSwapBuffers(m_window);
    }
}

void BlotApp::renderUI() {
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

    // File Browser Window
    if (showFileBrowser) {
        ImGui::Begin("File Browser", &showFileBrowser);
        // Use ImGui::FileBrowser for the file dialog
        static ImGui::FileBrowser fileDialog;
        fileDialog.Display();
        ImGui::End();
    }
    
    // Texture Viewer Window for Display
    if (m_uiManager->getWindowManager()->isWindowVisible("Texture")) {
        // Get texture viewer window through WindowManager
        auto textureWindow = std::dynamic_pointer_cast<blot::TextureViewerWindow>(m_uiManager->getWindowManager()->getWindow("Texture"));
        
        // Update canvas texture in the display window
        if (textureWindow) {
            auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
            if (canvasPtr && *canvasPtr) {
                unsigned int texId = (*canvasPtr)->getColorTexture();
                int width = (*canvasPtr)->getWidth();
                int height = (*canvasPtr)->getHeight();
                textureWindow->setTexture(texId, width, height);
            } else {
                textureWindow->clearTexture();
            }
        }
        
        // Render all windows through the window manager
        m_uiManager->getWindowManager()->renderAllWindows();
        
        // Handle input through the window manager
        m_uiManager->getWindowManager()->handleInput();
        
        // Render all windows through the window manager
        m_uiManager->getWindowManager()->renderAllWindows();
        
        // Handle input through the window manager
        m_uiManager->getWindowManager()->handleInput();
    }
    
    // Coordinate system debug info
    ImVec2 mousePos = ImGui::GetMousePos();
    ImVec2 screenPos = ImGui::GetIO().DisplaySize;
    
    // Get canvas coordinates if canvas window is open
    ImVec2 canvasPos = ImVec2(0, 0);
    ImVec2 canvasSize = ImVec2(0, 0);
    // TODO: Get actual canvas window position and size
    
    // Dropdown for coordinate system
    ImGui::Text("Coordinate System:");
    ImGui::SameLine();
    if (ImGui::BeginCombo("##CoordSystem", coordinateSystemNames[mouseCoordinateSystem])) {
        for (int i = 0; i < IM_ARRAYSIZE(coordinateSystemNames); i++) {
            const bool isSelected = (mouseCoordinateSystem == i);
            if (ImGui::Selectable(coordinateSystemNames[i], isSelected)) {
                mouseCoordinateSystem = i;
            }
            if (isSelected) {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    
    ImGui::Separator();
    
    // Update coordinate system with current information
    m_coordSystem.setScreenSize(screenPos.x, screenPos.y);
    
    // Get window position
    int windowX, windowY;
    glfwGetWindowPos(m_window, &windowX, &windowY);
    m_coordSystem.setAppWindow(windowX, windowY, m_windowWidth, m_windowHeight);
    
    // Get canvas window info if available
    auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
    if (canvasPtr && *canvasPtr) {
        auto graphics = (*canvasPtr)->getGraphics();
        if (graphics) {
            auto renderer = graphics->getRenderer();
            if (renderer) {
                float canvasWidth = static_cast<float>(renderer->getWidth());
                float canvasHeight = static_cast<float>(renderer->getHeight());
                m_coordSystem.setCanvasImage(canvasWidth, canvasHeight);
                
                // For now, use estimated canvas window position
                // This could be improved by tracking the actual canvas window position
                m_coordSystem.setCanvasWindow(windowX + 100, windowY + 100, 800, 600);
            }
        }
    }
    
    // Display coordinates based on selected system
    glm::vec2 mousePosGLM(mousePos.x, mousePos.y);
    auto coordInfo = m_coordSystem.getCoordinateInfo(mousePosGLM, static_cast<blot::CoordinateSpace>(mouseCoordinateSystem));
    
    ImGui::Text("%s Coordinates:", coordInfo.spaceName.c_str());
    ImGui::Text("  %s", coordInfo.description.c_str());
    ImGui::Text("  Mouse: (%.1f, %.1f)", coordInfo.mouse.x, coordInfo.mouse.y);
    ImGui::Text("  Relative: (%.1f, %.1f)", coordInfo.relative.x, coordInfo.relative.y);
    ImGui::Text("  Bounds: (%.0f, %.0f)", coordInfo.bounds.x, coordInfo.bounds.y);
    
    // Show additional info for specific coordinate systems
    switch (mouseCoordinateSystem) {
        case 0: { // Screen
            GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
            if (primaryMonitor) {
                const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
                if (mode) {
                    ImGui::Text("  Monitor: %dx%d @ %dHz", mode->width, mode->height, mode->refreshRate);
                }
            }
            break;
        }
        case 1: { // App
            ImGui::Text("  Window Pos: (%.0f, %.0f)", (float)windowX, (float)windowY);
            break;
        }
        case 3: { // Canvas
            auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
            if (canvasPtr && *canvasPtr) {
                auto graphics = (*canvasPtr)->getGraphics();
                if (graphics) {
                    auto renderer = graphics->getRenderer();
                    if (renderer) {
                        float canvasWidth = static_cast<float>(renderer->getWidth());
                        float canvasHeight = static_cast<float>(renderer->getHeight());
                        ImGui::Text("  Canvas Image: (%.0f, %.0f)", canvasWidth, canvasHeight);
                    }
                }
            }
            break;
        }
    }
    
    ImGui::Separator();
    
    // Mouse interaction state
    ImGui::Text("Mouse Interaction:");
    bool mouseDown = ImGui::IsMouseDown(ImGuiMouseButton_Left);
    bool mouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);
    bool mouseReleased = ImGui::IsMouseReleased(ImGuiMouseButton_Left);
    bool mouseDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    
    ImGui::Text("  Left Button: %s", mouseDown ? "DOWN" : "UP");
    ImGui::Text("  Clicked: %s", mouseClicked ? "YES" : "NO");
    ImGui::Text("  Released: %s", mouseReleased ? "YES" : "NO");
    ImGui::Text("  Dragging: %s", mouseDragging ? "YES" : "NO");
    
    // Mouse delta (movement)
    ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Left);
    ImGui::Text("  Drag Delta: (%.1f, %.1f)", mouseDelta.x, mouseDelta.y);
    
    // Tool state (TODO: Use ToolbarWindow)
    ImGui::Text("  Tool Active: TODO");
    ImGui::Text("  Tool Start: TODO");
    ImGui::Text("  Tool Current: TODO");
    
    ImGui::Separator();
    ImGui::Text("Debug Info:");
    ImGui::Text("  Delta Time: %.3f ms", m_deltaTime * 1000.0f);
    ImGui::Text("  Frame Rate: %.1f FPS", 1.0f / m_deltaTime);
    
    // Debug: Clear all shapes button
    if (ImGui::Button("Clear All Shapes")) {
        printf("[BlotApp] === Before Clear ===\n");
        printf("[BlotApp] Total entities: %zu\n", m_ecs.getEntityCount());
        auto allEntities = m_ecs.getAllEntities();
        for (auto entity : allEntities) {
            bool hasTransform = m_ecs.hasComponent<Transform>(entity);
            bool hasShape = m_ecs.hasComponent<blot::components::Shape>(entity);
            bool hasStyle = m_ecs.hasComponent<blot::components::Style>(entity);
            printf("[BlotApp] Entity %u: Transform=%d, Shape=%d, Style=%d\n", 
                   (unsigned int)entity, hasTransform, hasShape, hasStyle);
        }
        
        auto view = m_ecs.view<Transform, Shape, Style>();
        int count = 0;
        for (auto entity : view) {
            printf("[BlotApp] Destroying entity %u\n", (unsigned int)entity);
            m_ecs.destroyEntity(entity);
            count++;
        }
        printf("[BlotApp] Cleared %d shapes\n", count);
        
        printf("[BlotApp] === After Clear ===\n");
        printf("[BlotApp] Total entities: %zu\n", m_ecs.getEntityCount());
        allEntities = m_ecs.getAllEntities();
        for (auto entity : allEntities) {
            bool hasTransform = m_ecs.hasComponent<Transform>(entity);
            bool hasShape = m_ecs.hasComponent<blot::components::Shape>(entity);
            bool hasStyle = m_ecs.hasComponent<blot::components::Style>(entity);
            printf("[BlotApp] Entity %u: Transform=%d, Shape=%d, Style=%d\n", 
                   (unsigned int)entity, hasTransform, hasShape, hasStyle);
        }
    }
    
    // Debug: Create test shape button
    
    if (ImGui::Button("🎨 Random Theme")) {
        // Generate random colors for fun!
        auto& style = ImGui::GetStyle();
        auto& colors = style.Colors;
        
        // Random color generator
        auto randomColor = []() {
            return ImVec4(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                1.0f
            );
        };
        
        // Apply random colors to various ImGui elements
        colors[ImGuiCol_Text] = randomColor();
        colors[ImGuiCol_TextDisabled] = randomColor();
        colors[ImGuiCol_WindowBg] = randomColor();
        colors[ImGuiCol_ChildBg] = randomColor();
        colors[ImGuiCol_PopupBg] = randomColor();
        colors[ImGuiCol_Border] = randomColor();
        colors[ImGuiCol_BorderShadow] = randomColor();            colors[ImGuiCol_FrameBg] = randomColor();
            colors[ImGuiCol_FrameBgHovered] = randomColor();
            colors[ImGuiCol_FrameBgActive] = randomColor();
            colors[ImGuiCol_TitleBg] = randomColor();
            colors[ImGuiCol_TitleBgActive] = randomColor();
            colors[ImGuiCol_TitleBgCollapsed] = randomColor();
            colors[ImGuiCol_MenuBarBg] = randomColor();
            colors[ImGuiCol_ScrollbarBg] = randomColor();
            colors[ImGuiCol_ScrollbarGrab] = randomColor();
            colors[ImGuiCol_ScrollbarGrabHovered] = randomColor();
            colors[ImGuiCol_ScrollbarGrabActive] = randomColor();
            colors[ImGuiCol_CheckMark] = randomColor();
            colors[ImGuiCol_SliderGrab] = randomColor();
            colors[ImGuiCol_SliderGrabActive] = randomColor();
            colors[ImGuiCol_Button] = randomColor();
            colors[ImGuiCol_ButtonHovered] = randomColor();
            colors[ImGuiCol_ButtonActive] = randomColor();
            colors[ImGuiCol_Header] = randomColor();
            colors[ImGuiCol_HeaderHovered] = randomColor();
            colors[ImGuiCol_HeaderActive] = randomColor();
            colors[ImGuiCol_Separator] = randomColor();
            colors[ImGuiCol_SeparatorHovered] = randomColor();
            colors[ImGuiCol_SeparatorActive] = randomColor();
            colors[ImGuiCol_ResizeGrip] = randomColor();
            colors[ImGuiCol_ResizeGripHovered] = randomColor();
            colors[ImGuiCol_ResizeGripActive] = randomColor();
            colors[ImGuiCol_Tab] = randomColor();
            colors[ImGuiCol_TabHovered] = randomColor();
            colors[ImGuiCol_TabActive] = randomColor();
            colors[ImGuiCol_TabUnfocused] = randomColor();
            colors[ImGuiCol_TabUnfocusedActive] = randomColor();
            colors[ImGuiCol_PlotLines] = randomColor();
            colors[ImGuiCol_PlotLinesHovered] = randomColor();
            colors[ImGuiCol_PlotHistogram] = randomColor();
            colors[ImGuiCol_PlotHistogramHovered] = randomColor();
            colors[ImGuiCol_TableHeaderBg] = randomColor();
            colors[ImGuiCol_TableBorderLight] = randomColor();
            colors[ImGuiCol_TableBorderStrong] = randomColor();
            colors[ImGuiCol_TableRowBg] = randomColor();
            colors[ImGuiCol_TableRowBgAlt] = randomColor();
            colors[ImGuiCol_TextSelectedBg] = randomColor();
            colors[ImGuiCol_DragDropTarget] = randomColor();
            colors[ImGuiCol_NavHighlight] = randomColor();
            colors[ImGuiCol_NavWindowingHighlight] = randomColor();
            colors[ImGuiCol_NavWindowingDimBg] = randomColor();
            colors[ImGuiCol_ModalWindowDimBg] = randomColor();
            
            printf("[BlotApp] 🌈 Applied random theme! Colors are now completely chaotic!\n");
        }
        
        ImGui::SameLine();
        
        // Quick color presets
        if (ImGui::Button("🎨 Random Fill")) {
            ImVec4 randomFill = ImVec4(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                1.0f
            );
            m_toolbarWindow->setFillColor(randomFill);
            printf("[BlotApp] 🎨 Random fill color: (%.2f, %.2f, %.2f)\n", randomFill.x, randomFill.y, randomFill.z);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("⚫ No Fill")) {
            m_toolbarWindow->setFillColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            printf("[BlotApp] ⚫ Disabled fill\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🔴 Red Fill")) {
            m_toolbarWindow->setFillColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            printf("[BlotApp] 🔴 Set fill to red\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🟢 Green Fill")) {
            m_toolbarWindow->setFillColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            printf("[BlotApp] 🟢 Set fill to green\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🔵 Blue Fill")) {
            m_toolbarWindow->setFillColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
            printf("[BlotApp] 🔵 Set fill to blue\n");
        }
        
        ImGui::Separator();
        
        // Stroke color buttons
        if (ImGui::Button("⚫ No Stroke")) {
            m_toolbarWindow->setStrokeColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
            printf("[BlotApp] ⚫ Disabled stroke\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("⚪ White Stroke")) {
            m_toolbarWindow->setStrokeColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
            printf("[BlotApp] ⚪ Set stroke to white\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🔴 Red Stroke")) {
            m_toolbarWindow->setStrokeColor(ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
            printf("[BlotApp] 🔴 Set stroke to red\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🟢 Green Stroke")) {
            m_toolbarWindow->setStrokeColor(ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
            printf("[BlotApp] 🟢 Set stroke to green\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🔵 Blue Stroke")) {
            m_toolbarWindow->setStrokeColor(ImVec4(0.0f, 0.0f, 1.0f, 1.0f));
            printf("[BlotApp] 🔵 Set stroke to blue\n");
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("🎨 Random Stroke")) {
            ImVec4 randomStroke = ImVec4(
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                static_cast<float>(rand()) / RAND_MAX,
                1.0f
            );
            m_toolbarWindow->setStrokeColor(randomStroke);
            printf("[BlotApp] 🎨 Random stroke color: (%.2f, %.2f, %.2f)\n", randomStroke.x, randomStroke.y, randomStroke.z);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Create Test Shape")) {
            entt::entity shapeEntity = m_ecs.createEntity();
            
            // Add Transform component
            Transform transform;
            transform.x = 0.0f;
            transform.y = 0.0f;
            m_ecs.addComponent<Transform>(shapeEntity, transform);
            
            // Add Shape component with test coordinates
            Shape shape;
            shape.x1 = 100.0f; 
            shape.y1 = 100.0f; 
            shape.x2 = 200.0f; 
            shape.y2 = 200.0f;
            shape.type = blot::components::Shape::Type::Rectangle;
            m_ecs.addComponent<Shape>(shapeEntity, shape);
            
            // Add Style component
            ImVec4 fillColor = m_toolbarWindow->getFillColor();
            ImVec4 strokeColor = m_toolbarWindow->getStrokeColor();
            Style style;
            style.fillR = fillColor.x; style.fillG = fillColor.y;
            style.fillB = fillColor.z; style.fillA = fillColor.w;
            style.strokeR = strokeColor.x; style.strokeG = strokeColor.y;
            style.strokeB = strokeColor.z; style.strokeA = strokeColor.w;
            style.strokeWidth = m_toolbarWindow->getStrokeWidth();
            style.strokeCap = static_cast<blot::components::Style::StrokeCap>(0); // Default: Butt
            style.strokeJoin = static_cast<blot::components::Style::StrokeJoin>(0); // Default: Miter
            m_ecs.addComponent<Style>(shapeEntity, style);
            
            printf("[BlotApp] Created test shape with coordinates: x1=%.2f, y1=%.2f, x2=%.2f, y2=%.2f\n", 
                   shape.x1, shape.y1, shape.x2, shape.y2);
        }
        
        // Debug: Show all entities button
        if (ImGui::Button("Show All Entities")) {
            printf("[BlotApp] === Entity Status ===\n");
            printf("[BlotApp] Total entities: %zu\n", m_ecs.getEntityCount());
            auto allEntities = m_ecs.getAllEntities();
            for (auto entity : allEntities) {
                bool hasTransform = m_ecs.hasComponent<Transform>(entity);
                bool hasShape = m_ecs.hasComponent<blot::components::Shape>(entity);
                bool hasStyle = m_ecs.hasComponent<blot::components::Style>(entity);
                bool hasTexture = m_ecs.hasComponent<TextureComponent>(entity);
                printf("[BlotApp] Entity %u: Transform=%d, Shape=%d, Style=%d, Texture=%d\n", 
                       (unsigned int)entity, hasTransform, hasShape, hasStyle, hasTexture);
                
                if (hasShape) {
                    auto& shape = m_ecs.getComponent<blot::components::Shape>(entity);
                    printf("[BlotApp]   Shape coords: x1=%.2f, y1=%.2f, x2=%.2f, y2=%.2f\n", 
                           shape.x1, shape.y1, shape.x2, shape.y2);
                }
            }
            printf("[BlotApp] ====================\n");
        }
        
        // Debug: Force clear all entities except canvas
        if (ImGui::Button("Force Clear All")) {
            printf("[BlotApp] === Force Clearing All Entities ===\n");
            auto allEntities = m_ecs.getAllEntities();
            for (auto entity : allEntities) {
                if (entity != entt::null && entity != m_activeCanvasId) { // Don't destroy the canvas entity
                    printf("[BlotApp] Force destroying entity %u\n", (unsigned int)entity);
                    m_ecs.destroyEntity(entity);
                }
            }
            printf("[BlotApp] Force clear complete\n");
        }
        
        ImGui::End();
    }
    
    // Render all windows through the window manager
    m_uiManager->getWindowManager()->renderAllWindows();
    

    

    if (showImGuiMarkdownDemo) {
        ImGui::Begin("ImGui Markdown Demo", &showImGuiMarkdownDemo);
        ImGui::MarkdownConfig mdConfig; // Use default config or customize as needed
        if (!loadedMarkdown.empty()) {
            ImGui::Markdown(loadedMarkdown.c_str(), loadedMarkdown.size(), mdConfig);
        } else {
            static const char* markdown =
                "# ImGui Markdown Demo\n"
                "This is a **markdown** _demo_ for [ImGui Markdown](https://github.com/enkisoftware/imgui_markdown).\n\n"
                "- Bullet 1\n"
                "- Bullet 2\n\n"
                "`inline code`\n\n"
                "[ImPlot](https://github.com/epezent/implot)\n";
            ImGui::Markdown(markdown, strlen(markdown), mdConfig);
        }
        ImGui::End();
    }

    // ImPlot Demo Window - DISABLED to fix canvas layout issues
    // if (showImPlotDemo) {
    //     ImPlot::ShowDemoWindow(&showImPlotDemo);
    // }
    
    // DISABLE ALL DEMO WINDOWS to fix canvas layout issues
    showImPlotDemo = false;
    m_settings.showImPlotDemo = false;

    // Markdown Editor/Viewer windows (docked)
    if (showMarkdownEditor || showMarkdownViewer) {
        ImGui::SetNextWindowDockID(ImGui::GetID("MarkdownDockSpace"), ImGuiCond_Once);
        ImGui::DockSpace(ImGui::GetID("MarkdownDockSpace"), ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    }
    if (showMarkdownEditor) {
        ImGui::SetNextWindowDockID(ImGui::GetID("MarkdownDockSpace"), ImGuiCond_Once);
        ImGui::Begin("Markdown Editor", &showMarkdownEditor);
        ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput | ImGuiInputTextFlags_CallbackResize;
        static std::vector<char> buffer;
        if (buffer.empty() || buffer.size() < markdownEditorBuffer.size() + 1024) buffer.resize(markdownEditorBuffer.size() + 1024);
        std::copy(markdownEditorBuffer.begin(), markdownEditorBuffer.end(), buffer.begin());
        buffer[markdownEditorBuffer.size()] = '\0';
        if (ImGui::InputTextMultiline("##MarkdownEdit", buffer.data(), buffer.size(), ImVec2(-1, -1), flags)) {
            markdownEditorBuffer = buffer.data();
        }
        ImGui::End();
    }
    if (showMarkdownViewer) {
        ImGui::SetNextWindowDockID(ImGui::GetID("MarkdownDockSpace"), ImGuiCond_Once);
        ImGui::Begin("Markdown Viewer", &showMarkdownViewer);
        ImGui::MarkdownConfig mdConfig; // Use default config or customize as needed
        ImGui::Markdown(markdownEditorBuffer.c_str(), markdownEditorBuffer.size(), mdConfig);
        ImGui::End();
    }

    // Swatches window/component - DISABLED (not properly integrated)
    // TODO: Implement swatches functionality

void BlotApp::renderCanvas() {
    // Render only the active canvas if it exists
    if (m_activeCanvasId != entt::null && m_ecs.hasComponent<TextureComponent>(m_activeCanvasId)) {
        auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
        if (canvasPtr && *canvasPtr) {
            (*canvasPtr)->render();
        }
    }
}

void BlotApp::renderAddonUI() {
    // Render addon-specific UI
    if (m_addonManager) {
        m_addonManager->drawAll();
    }
}

void BlotApp::handleInput() {
    // Handle keyboard shortcuts
    if (ImGui::IsKeyDown(ImGuiKey_F5)) {
        m_scriptEngine->runCode(m_codeEditor->getCode());
    }
    
    if (ImGui::IsKeyDown(ImGuiKey_F6)) {
        m_scriptEngine->stop();
    }
    
    if (ImGui::IsKeyDown(ImGuiKey_F7)) {
        m_uiManager->getWindowManager()->toggleWindow("AddonManager");
    }

    // Removed ImGui window/mouse code from here. If you need mouse position relative to a window, do it in the UI rendering code.
    if (m_toolbarWindow->getCurrentTool() == BlotApp::ToolType::Ellipse) {
        // This logic should also be moved to the UI code if it depends on ImGui window state.
        // For now, leave as is if it does not use ImGui functions.
        // If it does, move it to renderUI or renderCanvas.
    }
}

void BlotApp::update() {
    // Update application logic
            CanvasUpdateSystem(m_ecs, m_resourceManager.get(), m_deltaTime);
    m_scriptEngine->update(m_deltaTime);
    if (m_addonManager) {
        m_addonManager->updateAll(m_deltaTime);
    }
    
    // Update ECS systems
    m_ecs.updateSystems(m_deltaTime);
    
    // Update window manager
    if (m_uiManager) {
        m_uiManager->getWindowManager()->update();
    }
} 

void BlotApp::switchRenderer(RendererType type) {
    m_currentRendererType = type;
    // TODO: Add logic to actually switch the renderer instance if needed
} 

void BlotApp::setImGuiTheme(ImGuiTheme theme) {
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

void BlotApp::loadTheme(const std::string& path) {
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

void BlotApp::saveCurrentTheme(const std::string& path) {
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

void BlotApp::configureMainMenuBarCallbacks() {
    auto mainMenuBar = getMainMenuBar();
    if (!mainMenuBar) return;
    
    // Set up callbacks
    mainMenuBar->setQuitCallback([this]() { m_running = false; });
    mainMenuBar->setNewSketchCallback([this]() { m_codeEditor->newSketch(); });
    mainMenuBar->setOpenSketchCallback([this]() { m_codeEditor->openSketch(); });
    mainMenuBar->setSaveSketchCallback([this]() { m_codeEditor->saveSketch(); });
    mainMenuBar->setRunSketchCallback([this]() { m_scriptEngine->runCode(m_codeEditor->getCode()); });
    mainMenuBar->setStopSketchCallback([this]() { m_scriptEngine->stop(); });
    mainMenuBar->setAddonManagerCallback([this]() { m_uiManager->getWindowManager()->toggleWindow("AddonManager"); });
    mainMenuBar->setReloadAddonsCallback([this]() {
        m_addonManager->cleanupAll();
        loadDefaultAddons();
        m_addonManager->initAll();
    });
    mainMenuBar->setThemeEditorCallback([this]() { m_uiManager->getWindowManager()->toggleWindow("ThemeEditor"); });
    
    // Canvas callbacks
    mainMenuBar->setNewCanvasCallback([this]() {
        int w = 1280, h = 720;
        auto newId = m_ecs.createEntity();
        TextureComponent texComp;
        texComp.width = w;
        texComp.height = h;
        texComp.renderTarget = true;
        m_ecs.addComponent<TextureComponent>(newId, texComp);
        (void)texComp; // Suppress unused variable warning
        // Create resources for this canvas entity using ResourceManager
        auto renderer = m_resourceManager->createRenderer(newId, w, h);
        if (renderer) {
            auto graphics = std::make_shared<Graphics>();
            graphics->setRenderer(renderer.get());
            auto canvas = std::make_unique<Canvas>(w, h, graphics);
            canvas->setECSManager(&m_ecs); // Connect Canvas to ECS
            m_resourceManager->addGraphics(newId, graphics);
            m_resourceManager->addCanvas(newId, std::move(canvas));
            m_activeCanvasId = newId;
            updateMainMenuBarCanvasList();
        }
    });
    
    mainMenuBar->setSaveCanvasCallback([this]() {
        auto result = pfd::save_file("Save Canvas As", ".", {"PNG Files (*.png)", "*.png", "All Files", "*"}).result();
        if (!result.empty() && m_ecs.hasComponent<TextureComponent>(m_activeCanvasId)) {
            auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
            if (canvasPtr && *canvasPtr) {
                (*canvasPtr)->saveFrame(result);
            }
        }
    });
    
    mainMenuBar->setCloseCanvasCallback([this](entt::entity entity) {
        m_ecs.destroyEntity(entity);
        m_resourceManager->destroyRenderer(entity);
        m_resourceManager->removeGraphics(entity);
        m_resourceManager->removeCanvas(entity);
        if (m_activeCanvasId == entity) {
            auto all = m_ecs.view<TextureComponent>();
            if (!all.empty())
                m_activeCanvasId = *all.begin();
            else
                m_activeCanvasId = entt::null;
        }
        updateMainMenuBarCanvasList();
    });
    
    mainMenuBar->setSelectCanvasCallback([this](entt::entity entity) {
        m_activeCanvasId = entity;
        updateMainMenuBarCanvasList();
    });
    
    // Markdown callbacks
    mainMenuBar->setOpenMarkdownCallback([this]() {
        auto result = pfd::open_file("Open Markdown File", ".", {"Markdown Files (*.md)", "*.md", "All Files", "*"}).result();
        if (!result.empty()) {
            std::ifstream in(result[0]);
            if (in) {
                markdownEditorBuffer.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                loadedMarkdownPath = result[0];
                showImGuiMarkdownDemo = true;
            }
        }
    });
    
    mainMenuBar->setSaveMarkdownCallback([this]() {
        auto result = pfd::save_file("Save Markdown As", ".", {"Markdown Files (*.md)", "*.md", "All Files", "*"}).result();
        if (!result.empty() && !markdownEditorBuffer.empty()) {
            std::ofstream out(result);
            if (out) {
                out << markdownEditorBuffer;
                loadedMarkdownPath = result;
            }
        }
    });
    
    // Demo callbacks
    mainMenuBar->setImPlotDemoCallback([this]() { showImPlotDemo = true; });
    mainMenuBar->setImGuiMarkdownDemoCallback([this]() { showImGuiMarkdownDemo = true; });
    mainMenuBar->setMarkdownEditorCallback([this]() { 
        showMarkdownEditor = true; 
        showMarkdownViewer = true; 
    });
    
    // Renderer callbacks
    mainMenuBar->setSwitchRendererCallback([this](int type) {
        switchRenderer(static_cast<RendererType>(type));
        m_graphics->setRenderer(m_currentRenderer.get());
    });
    mainMenuBar->setCurrentRendererType(static_cast<int>(m_currentRendererType));
    
    // Theme callbacks
    mainMenuBar->setSwitchThemeCallback([this](int theme) {
        setImGuiTheme(static_cast<ImGuiTheme>(theme));
    });
    mainMenuBar->setCurrentTheme(static_cast<int>(m_currentTheme));
    
    // Update canvas list
    updateMainMenuBarCanvasList();

    mainMenuBar->setNodeEditorCallback([this]() { m_windowManager->toggleWindow("NodeEditor"); });
}

void BlotApp::updateMainMenuBarCanvasList() {
    auto mainMenuBar = getMainMenuBar();
    if (!mainMenuBar) return;
    
    // Build canvas entities list
    std::vector<std::pair<entt::entity, std::string>> canvasEntities;
    auto view = m_ecs.view<TextureComponent>();
    for (auto entity : view) {
        char label[64];
        snprintf(label, sizeof(label), "Canvas %d%s", (int)entity, (entity == m_activeCanvasId ? " (active)" : ""));
        canvasEntities.emplace_back(entity, std::string(label));
    }
    
    mainMenuBar->setCanvasEntities(canvasEntities);
    mainMenuBar->setActiveCanvasId(m_activeCanvasId);
} 