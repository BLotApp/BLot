#pragma once

#include <memory>
#include <string>
#include <vector>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "rendering/Renderer.h"
#include "ui/ImGuiRenderer.h"
#include "ecs/ECSManager.h"
#include <unordered_map>
#include "rendering/Blend2DRenderer.h"
#include "rendering/ResourceManager.h"

#include "ui/CoordinateSystem.h"
#include "ui/UIManager.h"
#include "ui/windows/TextureViewerWindow.h"
#include "ui/windows/PropertiesWindow.h"
#include "ui/windows/ToolbarWindow.h"
#include "ui/windows/InfoWindow.h"
#include "ui/windows/MainMenuBar.h"
#include "ui/windows/CodeEditorWindow.h"
#include "ui/windows/AddonManagerWindow.h"
#include "ui/windows/NodeEditorWindow.h"
#include "ui/windows/ThemeEditorWindow.h"
#include "ui/windows/StrokeWindow.h"

#include "AppSettings.h"

// Forward declarations
class Graphics;
class TextRenderer;
class CodeEditor;
class ScriptEngine;
class AddonManager;

class BlotApp {
public:
    BlotApp();
    ~BlotApp();
    
    void run();
    
    // Debug mode control
    void setDebugMode(bool enabled) { m_debugMode = enabled; }
    bool getDebugMode() const { return m_debugMode; }
    void toggleDebugMode() { m_debugMode = !m_debugMode; }
    
    // Debug getters for troubleshooting
    GLFWwindow* getWindow() const { return m_window; }
    bool isRunning() const { return m_running; }
    
private:
    void initWindow();
    void initGraphics();
    void initAddons();
    void setup();
    void update();

    // App Window
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;

    // App settings
    AppSettings m_settings;

    // App state
    bool m_running;
    float m_deltaTime;
    float m_lastFrameTime;
    
    // Debug mode
    bool m_debugMode = false;
    
    // App Management
    std::unique_ptr<ECSManager> m_ecsManager;
    std::unique_ptr<AddonManager> m_addonManager;
    std::unique_ptr<ResourceManager> m_resourceManager;
    std::unique_ptr<blot::UIManager> m_uiManager;

    // App Systems
    blot::CoordinateSystem m_coordSystem;

    entt::entity m_activeCanvasId = entt::null; // Now refers to ECS entity
    std::shared_ptr<Graphics> m_graphics;

    std::unique_ptr<CodeEditor> m_codeEditor;
    std::unique_ptr<ScriptEngine> m_scriptEngine;

    // Node editor state (for NodeEditorWindow)
    std::vector<Node> m_nodes;
    int m_nextNodeId = 1;
}; 