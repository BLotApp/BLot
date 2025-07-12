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
#include "systems/ShapeRenderingSystem.h"
#include "ui/CoordinateSystem.h"
#include "ui/WindowManager.h"
#include "ui/CanvasDisplayWindow.h"
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
    void renderProperties();
    
    // Toolbar tool type and state (moved to public for external access)
    enum class ToolType { Select, Rectangle, Ellipse, Line, Polygon, Star, Pen };
    ToolType m_currentTool = ToolType::Select;
    ImVec2 m_toolStartPos = ImVec2(0,0);
    bool m_toolActive = false;

    // ECS-based shape management - no longer need local shape vector

    // Drag state for placing shapes
    bool m_isDragging = false;
    ImVec2 m_dragStart = ImVec2(0,0);
    ImVec2 m_dragEnd = ImVec2(0,0);
    
    // Node system for Node Editor
    struct NodeParam {
        std::string name;
        float value;
    };
    struct NodeConnection {
        int fromNodeId;
        std::string fromParam;
        int toNodeId;
        std::string toParam;
    };
    enum class NodeType { Circle, Add, Subtract, Multiply, Divide, Sin, Cos, Grid, Copy };
    struct Node {
        int id;
        NodeType type;
        std::vector<NodeParam> params;
        std::vector<NodeConnection> inputs;
        std::vector<NodeConnection> outputs;
    };
    std::vector<Node> m_nodes;
    int m_nextNodeId = 1;
    
    // Addon management
    void loadDefaultAddons();
    void renderAddonManager();
    void renderAddonList();
    void renderAddonDetails();
    
    // Renderer management
    void initRenderer();
    void switchRenderer(RendererType type);
    void renderRendererSettings();
    // m_graphics is always kept in sync with the current renderer via setRenderer.

    enum class ImGuiTheme { Dark, Light, Classic, Corporate, Dracula };
    ImGuiTheme m_currentTheme = ImGuiTheme::Dark;
    void setImGuiTheme(ImGuiTheme theme);
    void renderThemeEditor();
    void saveCurrentTheme(const std::string& path);
    std::string m_lastThemePath;
    void loadTheme(const std::string& path);
    bool m_showThemeEditor = false;

    // Resource management
    std::unique_ptr<ResourceManager> m_resourceManager;
    
    // Resource maps for ECS canvas entities (simplified)
    std::unordered_map<entt::entity, std::unique_ptr<Canvas>> m_canvasResources;
    std::unordered_map<entt::entity, std::shared_ptr<Graphics>> m_graphicsResources;

private:
    void initWindow();
    void initImGui();
    void initGraphics();
    void initAddons();
    void renderUI();
    void renderCanvas();
    void renderAddonUI();
    void handleInput();
    void update();

    // Window and OpenGL
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;

    // ImGui with enhanced text rendering
    std::unique_ptr<TextRenderer> m_textRenderer;
    std::unique_ptr<ImGuiRenderer> m_imguiRenderer;

    // ECS core for all entities/components (including canvases)
    ECSManager m_ecs;
    entt::entity m_activeCanvasId = entt::null; // Now refers to ECS entity
    std::shared_ptr<Graphics> m_graphics;
    std::unique_ptr<ShapeRenderingSystem> m_shapeRenderer;
    std::unique_ptr<CodeEditor> m_codeEditor;
    std::unique_ptr<ScriptEngine> m_scriptEngine;

    // Addon system
    std::unique_ptr<AddonManager> m_addonManager;

    // Renderer system
    std::unique_ptr<IRenderer> m_currentRenderer;
    RendererType m_currentRendererType = RendererType::Blend2D;
    std::vector<std::string> m_availableRenderers;

    // Application state
    bool m_running;
    float m_deltaTime;
    float m_lastFrameTime;

    // Application settings
    AppSettings m_settings;
    
    // Coordinate system utility
    blot::CoordinateSystem m_coordSystem;
    
    // UI state (now managed by AppSettings)
    bool m_showDemoWindow;
    bool m_showCodeEditor;
    bool m_showCanvas;
    bool m_showProperties;
    bool m_showAddonManager;
    bool m_showNodeEditor = false;
    // Toolbar and drawing mode
    bool m_showToolbar = true;
    bool m_drawCircleMode = false;
    
    // Window management (ECS-based)
    std::unique_ptr<blot::WindowManager> m_windowManager;
    std::shared_ptr<blot::CanvasDisplayWindow> m_canvasWindow;
}; 