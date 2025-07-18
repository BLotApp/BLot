#pragma once

#include "Window.h"
#include "core/canvas/Canvas.h"
#include "rendering/RenderingManager.h"
#include <entt/entt.hpp>
#include <memory>
#include "ecs/ECSManager.h"

namespace blot {

class ECSManager;

class CanvasWindow : public Window {
public:
    CanvasWindow(const std::string& title = "Canvas###MainCanvas", Flags flags = Flags::NoScrollbar | Flags::NoCollapse);
    virtual ~CanvasWindow() = default;

    // Canvas-specific methods
    void setRenderingManager(RenderingManager* renderingManager);
    void setECSManager(blot::ECSManager* ecs);
    void setActiveCanvasId(entt::entity canvasId);
    void setCurrentTool(int toolType);
    void setToolStartPos(const ImVec2& pos);
    void setToolActive(bool active);
    void setFillColor(const ImVec4& color);
    void setStrokeColor(const ImVec4& color);
    void setStrokeWidth(float width);

    // Mouse interaction
    void handleMouseInput();
    ImVec2 getCanvasMousePos() const;
    bool isMouseInsideCanvas() const;

    void renderContents() override;

private:
    // Canvas state
    RenderingManager* m_renderingManager = nullptr;
    blot::ECSManager* m_ecs = nullptr;
    entt::entity m_activeCanvasId = entt::null;
    
    // Tool state
    int m_currentTool = 0;
    ImVec2 m_toolStartPos = ImVec2(0, 0);
    bool m_toolActive = false;
    
    // Style state
    ImVec4 m_fillColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 m_strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_strokeWidth = 2.0f;
    
    // Mouse state
    ImVec2 m_canvasPos = ImVec2(0, 0);
    ImVec2 m_canvasSize = ImVec2(0, 0);
    ImVec2 m_canvasEnd = ImVec2(0, 0);
    ImVec2 m_canvasMousePos = ImVec2(0, 0);
    bool m_mouseInsideCanvas = false;
    
    // Helper methods
    void drawCanvasTexture();
    void handleShapeCreation();
    void createShape(const ImVec2& start, const ImVec2& end);
    ImVec2 convertToCanvasCoordinates(const ImVec2& screenPos) const;
    ImVec2 convertToBlend2DCoordinates(const ImVec2& canvasPos) const;
};

} // namespace blot 