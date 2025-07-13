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
#include "ecs/ECSManager.h"
#include "ui/CodeEditor.h"

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