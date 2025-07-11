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

// Use the new component types
using Shape = blot::components::Shape;
using Style = blot::components::Style;

inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
inline ImVec2 operator*(const ImVec2& a, float s) { return ImVec2(a.x * s, a.y * s); }
inline ImVec2 operator/(const ImVec2& a, float s) { return ImVec2(a.x / s, a.y / s); }

// Use BlotApp::ToolType everywhere
ImVec2 m_toolStartPos = ImVec2(0,0);
bool m_toolActive = false;

// Color picker variables
ImVec4 m_fillColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
ImVec4 m_strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

// Swatch data
std::vector<ImVec4> m_swatches = {
    ImVec4(0,0,0,1), ImVec4(1,1,1,1), ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(0,0,1,1)
};
int m_activeSwatchType = 0; // 0 = fill, 1 = stroke
std::string m_swatchesFile = "swatches.json";

void SaveSwatches(const std::string& path) {
    nlohmann::json j;
    for (const auto& c : m_swatches) {
        j.push_back({c.x, c.y, c.z, c.w});
    }
    std::ofstream out(path);
    out << j.dump(2);
}
void LoadSwatches(const std::string& path) {
    std::ifstream in(path);
    if (!in) return;
    nlohmann::json j;
    in >> j;
    m_swatches.clear();
    for (const auto& c : j) {
        if (c.is_array() && c.size() == 4)
            m_swatches.push_back(ImVec4(c[0], c[1], c[2], c[3]));
    }
}

// Add a member variable to control ImPlot demo window visibility
static bool showImPlotDemo = false;
static bool showImGuiMarkdownDemo = false;

// Mouse coordinate display
static bool showMouseCoordinates = true;
static int mouseCoordinateSystem = 0; // 0=Screen, 1=App, 2=Window
static const char* coordinateSystemNames[] = { "Screen", "App", "Window" };
static bool showMarkdownEditor = false;
static bool showMarkdownViewer = false;
static std::string markdownEditorBuffer;
static std::string loadedMarkdownPath;
static std::string loadedMarkdown;

bool showFileBrowser = false;
bool m_showSwatches = false;
bool m_showStrokePalette = false;
int m_selectedShape = 0; // 0=Rectangle, 1=Ellipse, 2=Line, 3=Polygon, 4=Star
float m_strokeWidth = 2.0f;
int m_strokeCap = 0; // 0=Butt, 1=Square, 2=Round
int m_strokeJoin = 0; // 0=Miter, 1=Bevel, 2=Round
float m_dashPattern[4] = {0};
int m_dashCount = 0;
float m_dashOffset = 0.0f;

// Add polygon/star sides control to stroke palette
static int m_polygonSides = 5;

// Add pen tool state variables at file scope
static std::vector<ImVec2> penPoints;
static std::vector<ImVec2> penHandles;
static bool penDraggingHandle = false;
static int penHandleIndex = -1;

// Add ToolButton helper at file scope
static void ToolButton(const char* icon, BlotApp::ToolType type) {
    ImGui::PushID(icon);
    extern BlotApp* g_blotAppInstance; // We'll set this pointer to the running app instance
    bool selected = (g_blotAppInstance && g_blotAppInstance->m_currentTool == type);
    if (selected) ImGui::PushStyleColor(ImGuiCol_Button, ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive));
    if (ImGui::Button(icon) && g_blotAppInstance) g_blotAppInstance->m_currentTool = type;
    if (selected) ImGui::PopStyleColor();
    ImGui::PopID();
    ImGui::SameLine();
}

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
    , m_showDemoWindow(false)
    , m_showCodeEditor(true)
    , m_showCanvas(true)
    , m_showProperties(true)
    , m_showAddonManager(false)
    , m_showNodeEditor(false)
    , m_showToolbar(true)
    , m_drawCircleMode(false)
    , m_showThemeEditor(false)
    , m_lastThemePath("theme.json")
{
    // Load application settings
    m_settings.loadSettings();
    
    initWindow();
    initImGui();
    initGraphics();
    initAddons();
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

void BlotApp::initImGui() {
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
    m_graphicsResources[m_activeCanvasId] = graphics;
    m_canvasResources[m_activeCanvasId] = std::move(canvas);
    
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

    // Ensure the code editor is always visible and dockable
    if (m_showCodeEditor) {
        ImGui::Begin("Code Editor", &m_showCodeEditor);
        m_codeEditor->render();
        ImGui::End();
    }

    // Main menu bar and windows
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("BLot")) {
            ImGui::Text("BLot");
            ImGui::Separator();
            if (ImGui::MenuItem("Preferences...")) {
                // TODO: Show preferences window
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Quit.")) {
                m_running = false;
            }
            // Add Samples submenu
            if (ImGui::BeginMenu("Samples")) {
                if (ImGui::MenuItem("ImPlot")) {
                    showImPlotDemo = true;
                }
                if (ImGui::MenuItem("ImGui Markdown Demo")) {
                    showImGuiMarkdownDemo = true;
                }
                if (ImGui::MenuItem("Markdown Editor/Viewer")) {
                    showMarkdownEditor = true;
                    showMarkdownViewer = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Renderer")) {
            if (ImGui::MenuItem("Blend2D", nullptr, m_currentRendererType == RendererType::Blend2D)) {
                switchRenderer(RendererType::Blend2D);
                m_graphics->setRenderer(m_currentRenderer.get());
            }
            if (ImGui::MenuItem("OpenGL/ES", nullptr, m_currentRendererType == RendererType::OpenGL)) {
                switchRenderer(RendererType::OpenGL);
                m_graphics->setRenderer(m_currentRenderer.get());
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Canvases")) {
            if (ImGui::MenuItem("New Canvas")) {
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
                    m_graphicsResources[newId] = graphics;
                    m_canvasResources[newId] = std::move(canvas);
                    m_activeCanvasId = newId;
                }
            }
            ImGui::Separator();
            auto view = m_ecs.view<TextureComponent>();
            for (auto entity : view) {
                auto& texComp = view.get<TextureComponent>(entity);
                char label[64];
                snprintf(label, sizeof(label), "Canvas %d%s", (int)entity, (entity == m_activeCanvasId ? " (active)" : ""));
                if (ImGui::Selectable(label, entity == m_activeCanvasId)) {
                    m_activeCanvasId = entity;
                }
                ImGui::SameLine();
                if (ImGui::SmallButton((std::string("x##close_") + std::to_string((int)entity)).c_str())) {
                    m_ecs.destroyEntity(entity);
                    m_resourceManager->destroyRenderer(entity);
                    m_graphicsResources.erase(entity);
                    m_canvasResources.erase(entity);
                    if (m_activeCanvasId == entity) {
                        auto all = m_ecs.view<TextureComponent>();
                        if (!all.empty())
                            m_activeCanvasId = *all.begin();
                        else
                            m_activeCanvasId = entt::null;
                    }
                    break;
                }
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New Sketch")) {
                m_codeEditor->newSketch();
            }
            if (ImGui::MenuItem("Open Sketch")) {
                m_codeEditor->openSketch();
            }
            if (ImGui::MenuItem("Save Sketch")) {
                m_codeEditor->saveSketch();
            }
            if (ImGui::MenuItem("Save As...")) {
                auto result = pfd::save_file("Save Canvas As", ".", {"PNG Files (*.png)", "*.png", "All Files", "*"}).result();
                if (!result.empty() && m_ecs.hasComponent<TextureComponent>(m_activeCanvasId)) {
                    auto it = m_canvasResources.find(m_activeCanvasId);
                    if (it != m_canvasResources.end()) {
                        it->second->saveFrame(result);
                    }
                }
            }
            if (ImGui::MenuItem("Open Markdown...")) {
                auto result = pfd::open_file("Open Markdown File", ".", {"Markdown Files (*.md)", "*.md", "All Files", "*"}).result();
                if (!result.empty()) {
                    std::ifstream in(result[0]);
                    if (in) {
                        markdownEditorBuffer.assign((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
                        loadedMarkdownPath = result[0];
                        showImGuiMarkdownDemo = true;
                    }
                }
            }
            if (ImGui::MenuItem("Save Markdown As...")) {
                auto result = pfd::save_file("Save Markdown As", ".", {"Markdown Files (*.md)", "*.md", "All Files", "*"}).result();
                if (!result.empty() && !markdownEditorBuffer.empty()) {
                    std::ofstream out(result);
                    if (out) {
                        out << markdownEditorBuffer;
                        loadedMarkdownPath = result;
                    }
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit")) {
                m_running = false;
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit")) {
            // Add edit actions here
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Windows")) {
            ImGui::MenuItem("Code Editor", nullptr, &m_showCodeEditor);
            ImGui::MenuItem("Canvas", nullptr, &m_showCanvas);
            ImGui::MenuItem("Properties", nullptr, &m_showProperties);
            ImGui::MenuItem("Addon Manager", nullptr, &m_showAddonManager);
            ImGui::MenuItem("Node Editor", nullptr, &m_showNodeEditor);
            ImGui::MenuItem("ImGui Demo", nullptr, &m_showDemoWindow);
            ImGui::MenuItem("File Browser", nullptr, &showFileBrowser);
            ImGui::MenuItem("Info Panel", nullptr, &showMouseCoordinates);
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Run")) {
            if (ImGui::MenuItem("Run Sketch")) {
                m_scriptEngine->runCode(m_codeEditor->getCode());
            }
            if (ImGui::MenuItem("Stop")) {
                m_scriptEngine->stop();
            }
            ImGui::EndMenu();
        }
        
        if (ImGui::BeginMenu("Addons")) {
            if (ImGui::MenuItem("Addon Manager")) {
                m_showAddonManager = true;
            }
            if (ImGui::MenuItem("Reload Addons")) {
                // Reload all addons
                m_addonManager->cleanupAll();
                loadDefaultAddons();
                m_addonManager->initAll();
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Theme")) {
            if (ImGui::MenuItem("Dark", nullptr, m_currentTheme == ImGuiTheme::Dark)) setImGuiTheme(ImGuiTheme::Dark);
            if (ImGui::MenuItem("Light", nullptr, m_currentTheme == ImGuiTheme::Light)) setImGuiTheme(ImGuiTheme::Light);
            if (ImGui::MenuItem("Classic", nullptr, m_currentTheme == ImGuiTheme::Classic)) setImGuiTheme(ImGuiTheme::Classic);
            if (ImGui::MenuItem("Corporate", nullptr, m_currentTheme == ImGuiTheme::Corporate)) setImGuiTheme(ImGuiTheme::Corporate);
            if (ImGui::MenuItem("Dracula", nullptr, m_currentTheme == ImGuiTheme::Dracula)) setImGuiTheme(ImGuiTheme::Dracula);
            ImGui::Separator();
            if (ImGui::MenuItem("Theme Editor...")) m_showThemeEditor = true;
            ImGui::EndMenu();
        }
        
        ImGui::EndMainMenuBar();
    }
    
    // File Browser Window
    if (showFileBrowser) {
        ImGui::Begin("File Browser", &showFileBrowser);
        // Use ImGui::FileBrowser for the file dialog
        static ImGui::FileBrowser fileDialog;
        fileDialog.Display();
        ImGui::End();
    }
    
    // Canvas Window for Drawing
    if (m_showCanvas) {
        ImGui::Begin("Canvas", &m_showCanvas);
        
        // Get the canvas area
        ImVec2 canvasPos = ImGui::GetCursorScreenPos();
        ImVec2 canvasSize = ImGui::GetContentRegionAvail();
        ImVec2 canvasEnd = ImVec2(canvasPos.x + canvasSize.x, canvasPos.y + canvasSize.y);
        
        // Draw the Blend2D canvas texture
        auto it = m_canvasResources.find(m_activeCanvasId);
        if (it != m_canvasResources.end()) {
            unsigned int texId = it->second->getColorTexture();
            printf("[ImGui] Displaying texture: ID=%u, size=%.1fx%.1f\n", texId, canvasSize.x, canvasSize.y);
            ImGui::Image((void*)(intptr_t)texId, canvasSize);
        } else {
            printf("[ImGui] ERROR: No canvas resource found for active canvas\n");
        }
        
        // Handle mouse input for drawing
        if (ImGui::IsWindowHovered()) {
            ImVec2 mousePos = ImGui::GetMousePos();
            printf("[Mouse] Window hovered, mousePos=(%.1f,%.1f), canvasBounds=(%.1f,%.1f)-(%.1f,%.1f)\n", 
                   mousePos.x, mousePos.y, canvasPos.x, canvasPos.y, canvasEnd.x, canvasEnd.y);
            if (mousePos.x >= canvasPos.x && mousePos.x <= canvasEnd.x && 
                mousePos.y >= canvasPos.y && mousePos.y <= canvasEnd.y) {
                printf("[Mouse] Inside canvas bounds!\n");
                
                // Convert to canvas coordinates
                ImVec2 canvasMousePos = ImVec2(mousePos.x - canvasPos.x, mousePos.y - canvasPos.y);
                printf("[Mouse] mousePos=(%.1f,%.1f), canvasPos=(%.1f,%.1f), canvasMousePos=(%.1f,%.1f)\n", 
                       mousePos.x, mousePos.y, canvasPos.x, canvasPos.y, canvasMousePos.x, canvasMousePos.y);
                
                // Pen tool logic
                if (m_currentTool == BlotApp::ToolType::Pen) {
                    // Add point on click
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        if (!penDraggingHandle) {
                            penPoints.push_back(canvasMousePos);
                            // Add default handles for cubic Bezier
                            penHandles.push_back(canvasMousePos + ImVec2(40, 0));
                            penHandles.push_back(canvasMousePos - ImVec2(40, 0));
                        }
                    }
                    // Drag handles
                    for (int i = 0; i < penHandles.size(); ++i) {
                        ImVec2 handleScreen = ImVec2(canvasPos.x + penHandles[i].x, canvasPos.y + penHandles[i].y);
                        if (!penDraggingHandle && ImGui::IsMouseHoveringRect(handleScreen - ImVec2(5,5), handleScreen + ImVec2(5,5)) && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                            penDraggingHandle = true;
                            penHandleIndex = i;
                        }
                    }
                    if (penDraggingHandle && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        penHandles[penHandleIndex] = canvasMousePos;
                    } else if (penDraggingHandle && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
                        penDraggingHandle = false;
                        penHandleIndex = -1;
                    }
                    // Draw points and handles
                    for (int i = 0; i < penPoints.size(); ++i) {
                        ImVec2 pt = ImVec2(canvasPos.x + penPoints[i].x, canvasPos.y + penPoints[i].y);
                        ImGui::GetWindowDrawList()->AddCircleFilled(pt, 4, IM_COL32(0,0,0,255));
                        if (i*2+1 < penHandles.size()) {
                            ImVec2 h1 = ImVec2(canvasPos.x + penHandles[i*2].x, canvasPos.y + penHandles[i*2].y);
                            ImVec2 h2 = ImVec2(canvasPos.x + penHandles[i*2+1].x, canvasPos.y + penHandles[i*2+1].y);
                            ImGui::GetWindowDrawList()->AddCircleFilled(h1, 3, IM_COL32(100,100,255,255));
                            ImGui::GetWindowDrawList()->AddCircleFilled(h2, 3, IM_COL32(100,100,255,255));
                            ImGui::GetWindowDrawList()->AddLine(pt, h1, IM_COL32(100,100,255,255), 1.0f);
                            ImGui::GetWindowDrawList()->AddLine(pt, h2, IM_COL32(100,100,255,255), 1.0f);
                        }
                    }
                    // Draw Bezier curve
                    for (int i = 0; i+1 < penPoints.size(); ++i) {
                        ImVec2 p0 = penPoints[i];
                        ImVec2 p1 = penHandles[i*2];
                        ImVec2 p2 = penHandles[i*2+1];
                        ImVec2 p3 = penPoints[i+1];
                        ImGui::GetWindowDrawList()->AddBezierCubic(
                            ImVec2(canvasPos.x + p0.x, canvasPos.y + p0.y),
                            ImVec2(canvasPos.x + p1.x, canvasPos.y + p1.y),
                            ImVec2(canvasPos.x + p2.x, canvasPos.y + p2.y),
                            ImVec2(canvasPos.x + p3.x, canvasPos.y + p3.y),
                            ImGui::ColorConvertFloat4ToU32(m_strokeColor), m_strokeWidth, 0
                        );
                    }
                    // Finish path on right-click
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
                        penPoints.clear();
                        penHandles.clear();
                        penDraggingHandle = false;
                        penHandleIndex = -1;
                    }
                } else {
                    // Other shapes
                    if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
                        m_toolStartPos = canvasMousePos;
                        m_toolActive = true;
                    }
                                    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_toolActive) {
                    printf("[Mouse] Left mouse released, creating shape...\n");
                    ImVec2 start = m_toolStartPos;
                    ImVec2 end = canvasMousePos;
                    float x1 = std::min(start.x, end.x);
                    float y1 = std::min(start.y, end.y);
                    float x2 = std::max(start.x, end.x);
                    float y2 = std::max(start.y, end.y);
                        
                        // Scale coordinates from ImGui window to Blend2D canvas
                        // Get actual canvas dimensions from the canvas resource
                        auto canvasIt = m_canvasResources.find(m_activeCanvasId);
                        float canvasWidth = 1280.0f;  // Default fallback
                        float canvasHeight = 720.0f;   // Default fallback
                        if (canvasIt != m_canvasResources.end()) {
                            // Get the actual canvas dimensions from the Graphics object
                            auto graphics = canvasIt->second->getGraphics();
                            if (graphics) {
                                auto renderer = graphics->getRenderer();
                                if (renderer) {
                                    canvasWidth = static_cast<float>(renderer->getWidth());
                                    canvasHeight = static_cast<float>(renderer->getHeight());
                                }
                            }
                        }
                        
                        float scaleX = canvasWidth / canvasSize.x;
                        float scaleY = canvasHeight / canvasSize.y;
                        
                        printf("[Canvas] Coordinate scaling: ImGui(%.1f,%.1f) -> Blend2D(%.1f,%.1f), scale=(%.2f,%.2f), canvas=%.0fx%.0f\n", 
                               x1, y1, x1 * scaleX, y1 * scaleY, scaleX, scaleY, canvasWidth, canvasHeight);
                        
                        // Create ECS entity with shape components
                        entt::entity shapeEntity = m_ecs.createEntity();
                        
                        // Add Transform component
                        Transform transform;
                        transform.x = 0.0f;
                        transform.y = 0.0f;
                        m_ecs.addComponent<Transform>(shapeEntity, transform);
                        
                        // Add Shape component with scaled coordinates
                        Shape shape;
                        shape.x1 = x1 * scaleX; 
                        shape.y1 = y1 * scaleY; 
                        shape.x2 = x2 * scaleX; 
                        shape.y2 = y2 * scaleY;
                        // Use the new component namespace
                        if (m_selectedShape == 0) shape.type = blot::components::Shape::Type::Rectangle;
                        else if (m_selectedShape == 1) shape.type = blot::components::Shape::Type::Ellipse;
                        else if (m_selectedShape == 2) shape.type = blot::components::Shape::Type::Line;
                        else if (m_selectedShape == 3) shape.type = blot::components::Shape::Type::Polygon;
                        else if (m_selectedShape == 4) shape.type = blot::components::Shape::Type::Star;
                        shape.sides = m_polygonSides;
                        m_ecs.addComponent<Shape>(shapeEntity, shape);
                        
                        // Add Style component
                        Style style;
                        style.fillR = m_fillColor.x; style.fillG = m_fillColor.y; 
                        style.fillB = m_fillColor.z; style.fillA = m_fillColor.w;
                        style.strokeR = m_strokeColor.x; style.strokeG = m_strokeColor.y;
                        style.strokeB = m_strokeColor.z; style.strokeA = m_strokeColor.w;
                        style.strokeWidth = m_strokeWidth;
                        style.strokeCap = static_cast<blot::components::Style::StrokeCap>(m_strokeCap);
                        style.strokeJoin = static_cast<blot::components::Style::StrokeJoin>(m_strokeJoin);
                        m_ecs.addComponent<Style>(shapeEntity, style);
                        m_toolActive = false;
                    }
                    // Draw preview while dragging
                    if (m_toolActive && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        ImVec2 start = m_toolStartPos;
                        ImVec2 end = canvasMousePos;
                        float x1 = std::min(start.x, end.x);
                        float y1 = std::min(start.y, end.y);
                        float x2 = std::max(start.x, end.x);
                        float y2 = std::max(start.y, end.y);
                        if (m_selectedShape == 0) {
                            ImGui::GetWindowDrawList()->AddRect(
                                ImVec2(canvasPos.x + x1, canvasPos.y + y1),
                                ImVec2(canvasPos.x + x2, canvasPos.y + y2),
                                ImGui::ColorConvertFloat4ToU32(m_strokeColor),
                                0.0f, 0, 1.0f
                            );
                        } else if (m_selectedShape == 1) {
                            ImVec2 center = ImVec2((x1 + x2) * 0.5f, (y1 + y2) * 0.5f);
                            ImVec2 radius = ImVec2((x2 - x1) * 0.5f, (y2 - y1) * 0.5f);
                            DrawEllipse(ImGui::GetWindowDrawList(), ImVec2(canvasPos.x + center.x, canvasPos.y + center.y), radius.x, radius.y, ImGui::ColorConvertFloat4ToU32(m_strokeColor), 1.0f, m_polygonSides);
                        } else if (m_selectedShape == 2) {
                            ImGui::GetWindowDrawList()->AddLine(
                                ImVec2(canvasPos.x + start.x, canvasPos.y + start.y),
                                ImVec2(canvasPos.x + end.x, canvasPos.y + end.y),
                                ImGui::ColorConvertFloat4ToU32(m_strokeColor), 1.0f
                            );
                        }
                    }
                }
            }
        }
        
        ImGui::End();
    }
    
    // Info Panel
    if (showMouseCoordinates) {
        ImGui::Begin("Info Panel", &showMouseCoordinates, ImGuiWindowFlags_AlwaysAutoResize);
        
        ImVec2 mousePos = ImGui::GetMousePos();
        ImVec2 screenPos = ImGui::GetIO().DisplaySize;
        
        // Get canvas coordinates if canvas window is open
        ImVec2 canvasPos = ImVec2(0, 0);
        ImVec2 canvasSize = ImVec2(0, 0);
        if (m_showCanvas) {
            // We need to get the actual canvas position and size
            // For now, we'll use the window position
            canvasPos = ImGui::GetWindowPos();
            canvasSize = ImGui::GetContentRegionAvail();
        }
        
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
        
        // Display coordinates based on selected system
        switch (mouseCoordinateSystem) {
            case 0: // Screen
                ImGui::Text("Screen Coordinates:");
                ImGui::Text("  Mouse: (%.1f, %.1f)", mousePos.x, mousePos.y);
                ImGui::Text("  Screen: (%.0f, %.0f)", screenPos.x, screenPos.y);
                break;
                
            case 1: // App (relative to application window)
                ImGui::Text("App Coordinates:");
                ImGui::Text("  Mouse: (%.1f, %.1f)", mousePos.x, mousePos.y);
                ImGui::Text("  Window: (%.0f, %.0f)", m_windowWidth, m_windowHeight);
                break;
                
            case 2: // Window (relative to canvas window)
                ImGui::Text("Canvas Window Coordinates:");
                if (m_showCanvas) {
                    ImVec2 canvasMousePos = ImVec2(mousePos.x - canvasPos.x, mousePos.y - canvasPos.y);
                    ImGui::Text("  Mouse: (%.1f, %.1f)", canvasMousePos.x, canvasMousePos.y);
                    ImGui::Text("  Canvas: (%.0f, %.0f)", canvasSize.x, canvasSize.y);
                    
                    // Show scaled coordinates for Blend2D
                    auto canvasIt = m_canvasResources.find(m_activeCanvasId);
                    if (canvasIt != m_canvasResources.end()) {
                        auto graphics = canvasIt->second->getGraphics();
                        if (graphics) {
                            auto renderer = graphics->getRenderer();
                            if (renderer) {
                                float canvasWidth = static_cast<float>(renderer->getWidth());
                                float canvasHeight = static_cast<float>(renderer->getHeight());
                                float scaleX = canvasWidth / canvasSize.x;
                                float scaleY = canvasHeight / canvasSize.y;
                                
                                ImVec2 blend2DCoords = ImVec2(canvasMousePos.x * scaleX, canvasMousePos.y * scaleY);
                                ImGui::Text("  Blend2D: (%.1f, %.1f)", blend2DCoords.x, blend2DCoords.y);
                                ImGui::Text("  Scale: (%.2f, %.2f)", scaleX, scaleY);
                            }
                        }
                    }
                } else {
                    ImGui::Text("Canvas window not open");
                }
                break;
        }
        
        ImGui::Separator();
        ImGui::Text("Debug Info:");
        ImGui::Text("  Delta Time: %.3f ms", m_deltaTime * 1000.0f);
        ImGui::Text("  Frame Rate: %.1f FPS", 1.0f / m_deltaTime);
        
        ImGui::End();
    }
    
    if (m_showToolbar) {
        ImGui::Begin("Toolbar", &m_showToolbar, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        ImVec4 selectedColor = ImGui::GetStyleColorVec4(ImGuiCol_ButtonActive);
        ImVec4 normalColor = ImGui::GetStyleColorVec4(ImGuiCol_Button);
        // Shape tool with popup
        ImGui::PushID("ShapeTool");
        if (ImGui::Button(ICON_FA_SHAPES)) ImGui::OpenPopup("ShapeMenu");
        ImGui::SameLine();
        if (ImGui::BeginPopup("ShapeMenu")) {
            if (ImGui::Selectable("Rectangle", m_selectedShape == 0)) { m_selectedShape = 0; m_currentTool = BlotApp::ToolType::Rectangle; }
            if (ImGui::Selectable("Ellipse", m_selectedShape == 1)) { m_selectedShape = 1; m_currentTool = BlotApp::ToolType::Ellipse; }
            if (ImGui::Selectable("Line", m_selectedShape == 2)) { m_selectedShape = 2; m_currentTool = BlotApp::ToolType::Line; }
            if (ImGui::Selectable("Polygon", m_selectedShape == 3)) { m_selectedShape = 3; m_currentTool = BlotApp::ToolType::Polygon; }
            if (ImGui::Selectable("Star", m_selectedShape == 4)) { m_selectedShape = 4; m_currentTool = BlotApp::ToolType::Star; }
            ImGui::EndPopup();
        }
        ImGui::PopID();
        // Show current shape icon
        ImGui::SameLine();
        ToolButton(ICON_FA_MOUSE_POINTER, BlotApp::ToolType::Select);
        ToolButton(ICON_FA_SQUARE, BlotApp::ToolType::Rectangle);
        ToolButton(ICON_FA_CIRCLE, BlotApp::ToolType::Ellipse);
        ToolButton(ICON_FA_PEN, BlotApp::ToolType::Line);
        // Stroke palette icon
        if (ImGui::Button(ICON_FA_SLIDERS_H)) m_showStrokePalette = !m_showStrokePalette;
        ImGui::SameLine();
        // Palette icon for swatches
        if (ImGui::Button(ICON_FA_PALETTE)) m_showSwatches = !m_showSwatches;
        ImGui::SameLine();
        ImGui::NewLine();
        
        // Color pickers
        ImGui::Text("Fill:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##FillColor", (float*)&m_fillColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::SameLine();
        ImGui::Text("Stroke:");
        ImGui::SameLine();
        ImGui::ColorEdit4("##StrokeColor", (float*)&m_strokeColor, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel);
        ImGui::End();
    }
    
    if (m_showStrokePalette) {
        if (ImGui::Begin("Stroke Palette", &m_showStrokePalette)) {
            ImGui::SliderFloat("Width", &m_strokeWidth, 0.1f, 20.0f);
            ImGui::Combo("Cap", &m_strokeCap, "Butt\0Square\0Round\0");
            ImGui::Combo("Join", &m_strokeJoin, "Miter\0Bevel\0Round\0");
            ImGui::InputInt("Dash Count", &m_dashCount);
            for (int i = 0; i < m_dashCount && i < 4; ++i) {
                ImGui::InputFloat(("Dash[" + std::to_string(i) + "]").c_str(), &m_dashPattern[i]);
            }
            ImGui::SliderFloat("Dash Offset", &m_dashOffset, 0.0f, 100.0f);
            ImGui::InputInt("Polygon/Star Sides", &m_polygonSides);
            if (m_polygonSides < 3) m_polygonSides = 3;
        }
        ImGui::End();
    }
    
    // Properties Window
    if (m_showProperties) {
        ImGui::Begin("Properties", &m_showProperties);
        renderProperties();
        ImGui::End();
    }
    
    // Addon Manager Window
    if (m_showAddonManager) {
        ImGui::Begin("Addon Manager", &m_showAddonManager);
        renderAddonManager();
        ImGui::End();
    }

    // Node Editor Window
    if (m_showNodeEditor) {
        ImGui::Begin("Node Editor", &m_showNodeEditor);
        ed::Begin("MyNodeEditor");
        // Add node creation button
        if (ImGui::Button("Add Circle Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Circle;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 50} };
            m_nodes.push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Add Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Add;
            n.params = { {"a", 0}, {"b", 0}, {"out", 0} };
            m_nodes.push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Multiply Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Multiply;
            n.params = { {"a", 1}, {"b", 1}, {"out", 1} };
            m_nodes.push_back(n);
        }
        if (ImGui::Button("Add Sin Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Sin;
            n.params = { {"in", 0}, {"out", 0} };
            m_nodes.push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Cos Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Cos;
            n.params = { {"in", 0}, {"out", 0} };
            m_nodes.push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Grid Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Grid;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 30}, {"rows", 3}, {"cols", 3}, {"spacing", 60} };
            m_nodes.push_back(n);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Copy Node")) {
            Node n;
            n.id = m_nextNodeId++;
            n.type = NodeType::Copy;
            n.params = { {"x", 100}, {"y", 100}, {"radius", 30}, {"count", 5}, {"dx", 40}, {"dy", 0} };
            m_nodes.push_back(n);
        }
        // Draw nodes and pins
        std::unordered_map<int, std::unordered_map<std::string, ed::PinId>> nodeParamPins;
        for (auto& node : m_nodes) {
            ed::BeginNode(node.id);
            ImGui::Text("%s", node.type == NodeType::Circle ? "Circle" : node.type == NodeType::Add ? "Add" : node.type == NodeType::Multiply ? "Multiply" : node.type == NodeType::Sin ? "Sin" : node.type == NodeType::Cos ? "Cos" : node.type == NodeType::Grid ? "Grid" : node.type == NodeType::Copy ? "Copy" : "Other");
            for (auto& param : node.params) {
                ed::PinId pinId = (node.id << 8) | std::hash<std::string>{}(param.name);
                nodeParamPins[node.id][param.name] = pinId;
                // For math nodes, "out" is output, others are input
                bool isOutput = false;
                if ((node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) && param.name == "out")
                    isOutput = true;
                if ((node.type == NodeType::Circle || node.type == NodeType::Grid || node.type == NodeType::Copy) && param.name == "radius")
                    isOutput = true; // Optionally allow shape nodes to output their radius
                ed::BeginPin(pinId, isOutput ? ed::PinKind::Output : ed::PinKind::Input);
                ImGui::PushID(&param);
                ImGui::InputFloat(param.name.c_str(), &param.value);
                ImGui::PopID();
                ed::EndPin();
            }
            ed::EndNode();
        }
        // Draw links
        for (const auto& node : m_nodes) {
            for (const auto& conn : node.inputs) {
                ed::LinkId linkId = (conn.fromNodeId << 16) | conn.toNodeId;
                ed::PinId fromPin = nodeParamPins[conn.fromNodeId][conn.fromParam];
                ed::PinId toPin = nodeParamPins[conn.toNodeId][conn.toParam];
                ed::Link(linkId, fromPin, toPin);
            }
        }
        // Handle new links
        if (ed::BeginCreate()) {
            ed::PinId startPinId, endPinId;
            if (ed::QueryNewLink(&startPinId, &endPinId)) {
                if (startPinId && endPinId && startPinId != endPinId) {
                    // Find nodes/params for pins
                    int fromNode = static_cast<int>(startPinId.Get() >> 8);
                    int toNode = static_cast<int>(endPinId.Get() >> 8);
                    std::string fromParam, toParam;
                    bool fromIsOutput = false, toIsInput = false;
                    for (auto& node : m_nodes) {
                        if (node.id == fromNode) {
                            for (auto& param : node.params) {
                                if (ax::NodeEditor::PinId((fromNode << 8) | std::hash<std::string>{}(param.name)) == startPinId) {
                                    fromParam = param.name;
                                    if ((node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) && param.name == "out")
                                        fromIsOutput = true;
                                    if ((node.type == NodeType::Circle || node.type == NodeType::Grid || node.type == NodeType::Copy) && param.name == "radius")
                                        fromIsOutput = true;
                                }
                            }
                        }
                        if (node.id == toNode) {
                            for (auto& param : node.params) {
                                if (ax::NodeEditor::PinId((toNode << 8) | std::hash<std::string>{}(param.name)) == endPinId) {
                                    toParam = param.name;
                                    if (!(node.type == NodeType::Add || node.type == NodeType::Multiply || node.type == NodeType::Sin || node.type == NodeType::Cos) || param.name != "out")
                                        toIsInput = true;
                                }
                            }
                        }
                    }
                    // Only allow output->input
                    if (fromIsOutput && toIsInput) {
                        for (auto& node : m_nodes) {
                            if (node.id == toNode) {
                                node.inputs.push_back({fromNode, fromParam, toNode, toParam});
                            }
                        }
                        ed::AcceptNewItem();
                    }
                }
            }
        }
        ed::EndCreate();
        ed::End();
        ImGui::End();
    }
    
    // ImGui Demo Window
    if (m_showDemoWindow) {
        ImGui::ShowDemoWindow(&m_showDemoWindow);
    }

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

    if (m_showThemeEditor) {
        renderThemeEditor();
    }

    if (showImPlotDemo) {
        ImPlot::ShowDemoWindow(&showImPlotDemo);
    }

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

    // Swatches window/component
    if (m_showSwatches) {
        if (ImGui::Begin("Swatches", &m_showSwatches)) {
            ImGui::RadioButton("Fill", &m_activeSwatchType, 0); ImGui::SameLine();
            ImGui::RadioButton("Stroke", &m_activeSwatchType, 1);
            ImGui::SameLine();
            if (ImGui::Button("Save")) SaveSwatches(m_swatchesFile);
            ImGui::SameLine();
            if (ImGui::Button("Load")) LoadSwatches(m_swatchesFile);
            ImGui::Separator();
            // Drag-and-drop reordering
            int swatchesPerRow = 8;
            int swatchToMove = -1, swatchTarget = -1;
            for (int i = 0; i < m_swatches.size(); ++i) {
                if (i > 0 && i % swatchesPerRow == 0) ImGui::NewLine();
                ImGui::PushID(i);
                if (ImGui::ColorButton("##swatch", m_swatches[i], ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoDragDrop, ImVec2(24,24))) {
                    if (m_activeSwatchType == 0) m_fillColor = m_swatches[i];
                    else m_strokeColor = m_swatches[i];
                }
                // Drag source
                if (ImGui::BeginDragDropSource()) {
                    ImGui::SetDragDropPayload("SWATCH", &i, sizeof(int));
                    ImGui::ColorButton("##drag", m_swatches[i], ImGuiColorEditFlags_NoTooltip, ImVec2(24,24));
                    ImGui::EndDragDropSource();
                }
                // Drag target
                if (ImGui::BeginDragDropTarget()) {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("SWATCH")) {
                        int src = *(const int*)payload->Data;
                        swatchToMove = src;
                        swatchTarget = i;
                    }
                    ImGui::EndDragDropTarget();
                }
                if (ImGui::BeginPopupContextItem()) {
                    if (ImGui::Button("Remove")) {
                        m_swatches.erase(m_swatches.begin() + i);
                        ImGui::EndPopup();
                        ImGui::PopID();
                        break;
                    }
                    ImGui::EndPopup();
                }
                ImGui::PopID();
                ImGui::SameLine();
            }
            if (swatchToMove != -1 && swatchTarget != -1 && swatchToMove != swatchTarget) {
                auto c = m_swatches[swatchToMove];
                m_swatches.erase(m_swatches.begin() + swatchToMove);
                m_swatches.insert(m_swatches.begin() + swatchTarget, c);
            }
            ImGui::NewLine();
            if (ImGui::Button("Add Current Color")) {
                m_swatches.push_back(m_activeSwatchType == 0 ? m_fillColor : m_strokeColor);
            }
        }
        ImGui::End();
    }
}

void BlotApp::renderCanvas() {
    // Render only the active canvas if it exists
    if (m_activeCanvasId != entt::null && m_ecs.hasComponent<TextureComponent>(m_activeCanvasId)) {
        auto it = m_canvasResources.find(m_activeCanvasId);
        if (it != m_canvasResources.end()) {
            it->second->render();
        }
    }
}

void BlotApp::renderAddonUI() {
    // Render addon-specific UI
    if (m_addonManager) {
        m_addonManager->drawAll();
    }
}

void BlotApp::renderAddonManager() {
    if (!m_addonManager) return;
    
    if (ImGui::BeginTabBar("AddonManagerTabs")) {
        if (ImGui::BeginTabItem("Addons")) {
            renderAddonList();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details")) {
            renderAddonDetails();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

void BlotApp::renderAddonList() {
    auto addons = m_addonManager->getAllAddons();
    
    ImGui::Text("Available Addons (%zu)", addons.size());
    ImGui::Separator();
    
    for (const auto& addon : addons) {
        if (!addon) continue;
        
        ImGui::PushID(addon->getName().c_str());
        
        bool enabled = addon->isEnabled();
        if (ImGui::Checkbox(addon->getName().c_str(), &enabled)) {
            if (enabled) {
                m_addonManager->enableAddon(addon->getName());
            } else {
                m_addonManager->disableAddon(addon->getName());
            }
        }
        
        if (ImGui::IsItemHovered()) {
            ImGui::SetTooltip("%s\nVersion: %s\nAuthor: %s", 
                             addon->getDescription().c_str(),
                             addon->getVersion().c_str(),
                             addon->getAuthor().c_str());
        }
        
        ImGui::PopID();
    }
    
    ImGui::Separator();
    if (ImGui::Button("Reload Addons")) {
        m_addonManager->cleanupAll();
        loadDefaultAddons();
        m_addonManager->initAll();
    }
}

void BlotApp::renderAddonDetails() {
    auto addons = m_addonManager->getAllAddons();
    
    for (const auto& addon : addons) {
        if (!addon) continue;
        
        if (ImGui::CollapsingHeader(addon->getName().c_str())) {
            ImGui::Text("Version: %s", addon->getVersion().c_str());
            ImGui::Text("Description: %s", addon->getDescription().c_str());
            ImGui::Text("Author: %s", addon->getAuthor().c_str());
            ImGui::Text("License: %s", addon->getLicense().c_str());
            ImGui::Text("Status: %s", addon->isEnabled() ? "Enabled" : "Disabled");
            ImGui::Text("Initialized: %s", addon->isInitialized() ? "Yes" : "No");
            
            auto dependencies = addon->getDependencies();
            if (!dependencies.empty()) {
                ImGui::Text("Dependencies:");
                for (const auto& dep : dependencies) {
                    ImGui::BulletText("%s", dep.c_str());
                }
            }
        }
    }
}

void BlotApp::renderProperties() {
    static float strokeWidth = 1.0f;
    static float fillOpacity = 1.0f;
    static ImVec4 strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    static ImVec4 fillColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    static bool fillEnabled = true;
    static bool strokeEnabled = true;
    static int cap = 2; // 0=Butt, 1=Square, 2=Round
    static int join = 2; // 0=Miter, 1=Bevel, 2=Round
    static float dashPattern[4] = {0};
    static int dashCount = 0;
    static float dashOffset = 0.0f;

    ImGui::Text("Drawing Properties");
    ImGui::Separator();

    ImGui::Checkbox("Fill Enabled", &fillEnabled);
    ImGui::Checkbox("Stroke Enabled", &strokeEnabled);
    ImGui::SliderFloat("Stroke Width", &strokeWidth, 0.1f, 20.0f);
    ImGui::SliderFloat("Fill Opacity", &fillOpacity, 0.0f, 1.0f);
    ImGui::ColorEdit4("Stroke Color", (float*)&strokeColor);
    ImGui::ColorEdit4("Fill Color", (float*)&fillColor);
    ImGui::Combo("Stroke Cap", &cap, "Butt\0Square\0Round\0");
    ImGui::Combo("Stroke Join", &join, "Miter\0Bevel\0Round\0");
    ImGui::InputInt("Dash Count", &dashCount);
    for (int i = 0; i < dashCount && i < 4; ++i) {
        ImGui::InputFloat(("Dash[" + std::to_string(i) + "]").c_str(), &dashPattern[i]);
    }
    ImGui::SliderFloat("Dash Offset", &dashOffset, 0.0f, 100.0f);

    if (ImGui::Button("Apply to Canvas")) {
        m_graphics->setStrokeWidth(strokeWidth);
        m_graphics->setFillOpacity(fillOpacity);
        m_graphics->setStrokeColor(strokeColor.x, strokeColor.y, strokeColor.z, strokeColor.w);
        m_graphics->setFillColor(fillColor.x, fillColor.y, fillColor.z, fillColor.w);
        if (fillEnabled) m_graphics->setFillColor(fillColor.x, fillColor.y, fillColor.z, fillColor.w);
        else m_graphics->setFillOpacity(0.0f);
        if (strokeEnabled) m_graphics->setStrokeWidth(strokeWidth);
        else m_graphics->setStrokeWidth(0.0f);
        m_graphics->setStrokeCap(cap);
        m_graphics->setStrokeJoin(join);
        std::vector<float> dashes(dashPattern, dashPattern + dashCount);
        m_graphics->setStrokeDash(dashes, dashOffset);
    }

    ImGui::Separator();
    ImGui::Text("Addon System");
    ImGui::Text("Active Addons: %zu", m_addonManager ? m_addonManager->getEnabledAddons().size() : 0);
    ImGui::Text("Total Addons: %zu", m_addonManager ? m_addonManager->getAllAddons().size() : 0);
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
        m_showAddonManager = !m_showAddonManager;
    }

    // Removed ImGui window/mouse code from here. If you need mouse position relative to a window, do it in the UI rendering code.
    if (m_currentTool == BlotApp::ToolType::Ellipse) {
        // This logic should also be moved to the UI code if it depends on ImGui window state.
        // For now, leave as is if it does not use ImGui functions.
        // If it does, move it to renderUI or renderCanvas.
    }
}

void BlotApp::update() {
    // Update application logic
    CanvasUpdateSystem(m_ecs, m_canvasResources, m_deltaTime);
    m_scriptEngine->update(m_deltaTime);
    if (m_addonManager) {
        m_addonManager->updateAll(m_deltaTime);
    }
    
    // Update ECS systems
    m_ecs.updateSystems(m_deltaTime);
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

void BlotApp::renderThemeEditor() {
    static char savePath[256] = "theme.json";
    static char loadPath[256] = "theme.json";
    ImGui::Begin("ImGui Theme Editor", &m_showThemeEditor);
    ImGuiStyle& style = ImGui::GetStyle();
    for (int i = 0; i < ImGuiCol_COUNT; ++i) {
        ImGui::ColorEdit4(ImGui::GetStyleColorName(i), (float*)&style.Colors[i]);
    }
    ImGui::InputText("Save As", savePath, sizeof(savePath));
    if (ImGui::Button("Save Theme As")) {
        saveCurrentTheme(savePath);
        m_lastThemePath = savePath;
    }
    ImGui::InputText("Load From", loadPath, sizeof(loadPath));
    if (ImGui::Button("Load Theme")) {
        loadTheme(loadPath);
        m_lastThemePath = loadPath;
    }
    ImGui::End();
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
void CanvasUpdateSystem(ECSManager& ecs, std::unordered_map<entt::entity, std::unique_ptr<Canvas>>& canvasResources, float deltaTime) {
    auto view = ecs.view<TextureComponent>();
    for (auto entity : view) {
        auto it = canvasResources.find(entity);
        if (it != canvasResources.end()) {
            it->second->update(deltaTime);
        }
    }
}

// ECS-style system: render all canvases (optionally, only active)
void CanvasRenderSystem(ECSManager& ecs, std::unordered_map<entt::entity, std::unique_ptr<Canvas>>& canvasResources, entt::entity activeCanvasId) {
    if (activeCanvasId != entt::null && ecs.hasComponent<TextureComponent>(activeCanvasId)) {
        auto it = canvasResources.find(activeCanvasId);
        if (it != canvasResources.end()) {
            it->second->render();
        }
    }
} 