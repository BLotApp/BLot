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
#include "ui/UIManager.h"
#include "ui/TextureViewerWindow.h"
#include "ui/PropertiesWindow.h"
#include "ui/ToolbarWindow.h"
#include "ui/InfoWindow.h"
#include "ui/MainMenuBar.h"
#include "ui/CodeEditorWindow.h"
#include "ui/AddonManagerWindow.h"
#include "ui/NodeEditorWindow.h"
#include "ui/ThemeEditorWindow.h"
#include "ui/StrokeWindow.h"

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
    
    // UI Components
    std::shared_ptr<blot::ToolbarWindow> getToolbar() { 
        return std::dynamic_pointer_cast<blot::ToolbarWindow>(m_uiManager->getWindowManager()->getWindow("Toolbar")); 
    }
    std::shared_ptr<blot::InfoWindow> getInfoWindow() {
        return std::dynamic_pointer_cast<blot::InfoWindow>(m_uiManager->getWindowManager()->getWindow("InfoWindow"));
    }
    std::shared_ptr<blot::PropertiesWindow> getPropertiesWindow() { 
        return std::dynamic_pointer_cast<blot::PropertiesWindow>(m_uiManager->getWindowManager()->getWindow("Properties")); 
    }
    std::shared_ptr<blot::MainMenuBar> getMainMenuBar() { 
        return std::dynamic_pointer_cast<blot::MainMenuBar>(m_uiManager->getWindowManager()->getWindow("MainMenuBar")); 
    }
    std::shared_ptr<blot::CodeEditorWindow> getCodeEditorWindow() { 
        return std::dynamic_pointer_cast<blot::CodeEditorWindow>(m_uiManager->getWindowManager()->getWindow("CodeEditor")); 
    }
    std::shared_ptr<blot::AddonManagerWindow> getAddonManagerWindow() { 
        return std::dynamic_pointer_cast<blot::AddonManagerWindow>(m_uiManager->getWindowManager()->getWindow("AddonManager")); 
    }
    std::shared_ptr<blot::NodeEditorWindow> getNodeEditorWindow() { 
        return std::dynamic_pointer_cast<blot::NodeEditorWindow>(m_uiManager->getWindowManager()->getWindow("NodeEditor")); 
    }
    std::shared_ptr<blot::ThemeEditorWindow> getThemeEditorWindow() { 
        return std::dynamic_pointer_cast<blot::ThemeEditorWindow>(m_uiManager->getWindowManager()->getWindow("ThemeEditor")); 
    }
    std::shared_ptr<blot::ToolbarWindow> getToolbarWindow() const { return m_toolbarWindow; }
    std::shared_ptr<StrokeWindow> getStrokeWindow() const { return m_strokeWindow; }
    
    // Addon management
    void loadDefaultAddons();
    
    // Renderer management
    void initRenderer();
    void switchRenderer(RendererType type);
    void renderRendererSettings();
    // m_graphics is always kept in sync with the current renderer via setRenderer.

    enum class ImGuiTheme { Dark, Light, Classic, Corporate, Dracula };
    ImGuiTheme m_currentTheme = ImGuiTheme::Dark;
    void setImGuiTheme(ImGuiTheme theme);
    void saveCurrentTheme(const std::string& path);
    std::string m_lastThemePath;
    void loadTheme(const std::string& path);

    // Resource management
    std::unique_ptr<ResourceManager> m_resourceManager;

private:
    void initWindow();
    void initGraphics();
    void initAddons();
    void renderUI();
    void renderCanvas();
    void renderAddonUI();
    void handleInput();
    void update();
    void configureMainMenuBarCallbacks();
    void updateMainMenuBarCanvasList();

    // Window and OpenGL
    GLFWwindow* m_window;
    int m_windowWidth;
    int m_windowHeight;

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
    
    // UI management
    std::unique_ptr<blot::UIManager> m_uiManager;
    
    // Node editor state (for NodeEditorWindow)
    std::vector<blot::Node> m_nodes;
    int m_nextNodeId = 1;
    
    // Window management (ECS-based)
    std::shared_ptr<blot::ToolbarWindow> m_toolbarWindow;
    std::shared_ptr<StrokeWindow> m_strokeWindow;
}; 