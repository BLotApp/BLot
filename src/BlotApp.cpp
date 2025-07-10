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
#include "BlotApp.h"
#include "Canvas.h"
#include "Graphics.h"
#include "TextRenderer.h"
#include "ImGuiRenderer.h"
#include "CodeEditor.h"
#include "ScriptEngine.h"
#include "AddonManager.h"
#include "NodeEditor.h"
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

#include "../assets/fonts/fontRobotoRegular.h"

#ifdef _WIN32
#include <windows.h>
#endif

enum class ToolType { Select, Rectangle, Ellipse, Line };
ToolType m_currentTool = ToolType::Select;
ImVec2 m_toolStartPos = ImVec2(0,0);
bool m_toolActive = false;

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
    initWindow();
    initImGui();
    initGraphics();
    initAddons();
}

BlotApp::~BlotApp() {
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
    m_canvas = std::make_unique<Canvas>(m_windowWidth, m_windowHeight);
    m_graphics = std::make_unique<Graphics>();
    m_codeEditor = std::make_unique<CodeEditor>();
    m_scriptEngine = std::make_unique<ScriptEngine>();
    
    // Set up default creative coding environment
    m_codeEditor->loadDefaultTemplate();
    
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
    while (!glfwWindowShouldClose(m_window) && m_running) {
        float currentTime = static_cast<float>(glfwGetTime());
        m_deltaTime = currentTime - m_lastFrameTime;
        m_lastFrameTime = currentTime;
        
        glfwPollEvents();
        handleInput();
        update();
        
        // Start ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        renderUI();
        renderCanvas();
        renderAddonUI();
        
        // Render ImGui
        ImGui::Render();
        
        // Clear and render
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
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

    // Main menu bar and windows
    if (ImGui::BeginMainMenuBar()) {
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
    
    // Code Editor Window
    if (m_showCodeEditor) {
        ImGui::Begin("Code Editor", &m_showCodeEditor);
        m_codeEditor->render();
        ImGui::End();
    }
    
    // Canvas Window
    if (m_showCanvas) {
        ImGui::Begin("Canvas", &m_showCanvas);
        if (m_canvas) {
            ImTextureID tex_id = (ImTextureID)(intptr_t)m_canvas->getColorTexture();
            ImVec2 size((float)m_canvas->getWidth(), (float)m_canvas->getHeight());
            ImGui::Image(tex_id, size, ImVec2(0,1), ImVec2(1,0)); // Flip vertically if needed
        }
        ImGui::End();
    }

    if (m_showToolbar) {
        ImGui::Begin("Toolbar", &m_showToolbar, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Button(ICON_FA_MOUSE_POINTER)) m_currentTool = ToolType::Select;
        if (ImGui::Button(ICON_FA_SQUARE)) m_currentTool = ToolType::Rectangle;
        if (ImGui::Button(ICON_FA_CIRCLE)) m_currentTool = ToolType::Ellipse;
        if (ImGui::Button(ICON_FA_PEN)) m_currentTool = ToolType::Line;
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

    if (m_showThemeEditor) {
        renderThemeEditor();
    }
}

void BlotApp::renderCanvas() {
    // Canvas rendering is handled by the Canvas class
    // This is where the creative coding output is displayed
    m_shapes.clear();
    std::unordered_map<int, std::unordered_map<std::string, float>> nodeOutputs;
    // Evaluate math nodes first
    for (auto& node : m_nodes) {
        if (node.type == NodeType::Add) {
            float a = node.params[0].value;
            float b = node.params[1].value;
            nodeOutputs[node.id]["out"] = a + b;
        } else if (node.type == NodeType::Multiply) {
            float a = node.params[0].value;
            float b = node.params[1].value;
            nodeOutputs[node.id]["out"] = a * b;
        } else if (node.type == NodeType::Sin) {
            float in = node.params[0].value;
            nodeOutputs[node.id]["out"] = sinf(in);
        } else if (node.type == NodeType::Cos) {
            float in = node.params[0].value;
            nodeOutputs[node.id]["out"] = cosf(in);
        }
    }
    // Evaluate shape nodes, using connections
    for (auto& node : m_nodes) {
        if (node.type == NodeType::Circle) {
            float x = node.params[0].value;
            float y = node.params[1].value;
            float r = node.params[2].value;
            // Apply connections
            for (const auto& conn : node.inputs) {
                if (conn.toParam == "x") x = nodeOutputs[conn.fromNodeId][conn.fromParam];
                if (conn.toParam == "y") y = nodeOutputs[conn.fromNodeId][conn.fromParam];
                if (conn.toParam == "radius") r = nodeOutputs[conn.fromNodeId][conn.fromParam];
            }
            m_shapes.push_back({Shape::Type::Circle, ImVec2(x, y), r});
        } else if (node.type == NodeType::Grid) {
            float x = node.params[0].value;
            float y = node.params[1].value;
            float r = node.params[2].value;
            int rows = (int)node.params[3].value;
            int cols = (int)node.params[4].value;
            float spacing = node.params[5].value;
            for (int i = 0; i < rows; ++i) {
                for (int j = 0; j < cols; ++j) {
                    m_shapes.push_back({Shape::Type::Circle, ImVec2(x + j * spacing, y + i * spacing), r});
                }
            }
        } else if (node.type == NodeType::Copy) {
            float x = node.params[0].value;
            float y = node.params[1].value;
            float r = node.params[2].value;
            int count = (int)node.params[3].value;
            float dx = node.params[4].value;
            float dy = node.params[5].value;
            for (int i = 0; i < count; ++i) {
                m_shapes.push_back({Shape::Type::Circle, ImVec2(x + i * dx, y + i * dy), r});
            }
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

    ImGuiIO& io = ImGui::GetIO();
    ImGui::Begin("Canvas");
    ImVec2 canvasPos = ImGui::GetWindowPos();
    ImVec2 localPos = ImVec2(io.MousePos.x - canvasPos.x, io.MousePos.y - canvasPos.y);
    ImGui::End();
    if (m_currentTool == ToolType::Ellipse) {
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
            if (ImGui::IsMouseClicked(0) && !m_isDragging) {
                m_isDragging = true;
                m_dragStart = localPos;
                m_dragEnd = localPos;
            }
            if (m_isDragging && ImGui::IsMouseDown(0)) {
                m_dragEnd = localPos;
            }
            if (m_isDragging && ImGui::IsMouseReleased(0)) {
                float dx = m_dragEnd.x - m_dragStart.x;
                float dy = m_dragEnd.y - m_dragStart.y;
                float radius = sqrtf(dx*dx + dy*dy);
                m_shapes.push_back({Shape::Type::Circle, m_dragStart, radius});
                m_isDragging = false;
            }
        }
    }
}

void BlotApp::update() {
    // Update application logic
    m_canvas->update(m_deltaTime);
    m_scriptEngine->update(m_deltaTime);
    
    // Update addons
    if (m_addonManager) {
        m_addonManager->updateAll(m_deltaTime);
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