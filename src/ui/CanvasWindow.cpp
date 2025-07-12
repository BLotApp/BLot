#include "CanvasWindow.h"
#include "ImGuiWindow.h"
#include "canvas/Canvas.h"
#include "ecs/ECSManager.h"
#include "components/ShapeComponent.h"
#include "components/StyleComponent.h"
// Transform component is defined in ECSManager.h
#include "rendering/Graphics.h"
#include <imgui.h>
#include <iostream>

namespace blot {

CanvasWindow::CanvasWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
    // Set initial window size
    m_state.size = ImVec2(800, 600);
    m_state.position = ImVec2(100, 100);
}

void CanvasWindow::setResourceManager(ResourceManager* resourceManager) {
    m_resourceManager = resourceManager;
}

void CanvasWindow::setECSManager(ECSManager* ecs) {
    m_ecs = ecs;
}

void CanvasWindow::setActiveCanvasId(entt::entity canvasId) {
    m_activeCanvasId = canvasId;
}

void CanvasWindow::setCurrentTool(int toolType) {
    m_currentTool = toolType;
}

void CanvasWindow::setToolStartPos(const ImVec2& pos) {
    m_toolStartPos = pos;
}

void CanvasWindow::setToolActive(bool active) {
    m_toolActive = active;
}

void CanvasWindow::setFillColor(const ImVec4& color) {
    m_fillColor = color;
}

void CanvasWindow::setStrokeColor(const ImVec4& color) {
    m_strokeColor = color;
}

void CanvasWindow::setStrokeWidth(float width) {
    m_strokeWidth = width;
}

void CanvasWindow::handleMouseInput() {
    if (!ImGui::IsWindowHovered()) {
        return;
    }

    // Check if we're dragging the window (mouse over title bar)
    bool isDraggingWindow = ImGui::IsMouseDragging(ImGuiMouseButton_Left) && 
                          ImGui::GetIO().MouseClickedPos[0].y < ImGui::GetCursorPosY();
    
    // Always allow window dragging regardless of tool state
    if (isDraggingWindow) {
        // Window is being dragged, don't handle tool input
        return;
    }

    ImVec2 mousePos = ImGui::GetMousePos();
    
    // Check if mouse is inside canvas bounds
    m_mouseInsideCanvas = (mousePos.x >= m_canvasPos.x && mousePos.x <= m_canvasEnd.x && 
                          mousePos.y >= m_canvasPos.y && mousePos.y <= m_canvasEnd.y);
    
    if (m_mouseInsideCanvas) {
        m_canvasMousePos = convertToCanvasCoordinates(mousePos);
        handleShapeCreation();
    }
}

ImVec2 CanvasWindow::getCanvasMousePos() const {
    return m_canvasMousePos;
}

bool CanvasWindow::isMouseInsideCanvas() const {
    return m_mouseInsideCanvas;
}

void CanvasWindow::render() {
    // Set window size and position
    ImGui::SetNextWindowSize(m_state.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(m_state.position, ImGuiCond_FirstUseEver);
    
    // Begin the window
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        // Get the canvas area
        m_canvasPos = ImGui::GetCursorScreenPos();
        m_canvasSize = ImGui::GetContentRegionAvail();
        m_canvasEnd = ImVec2(m_canvasPos.x + m_canvasSize.x, m_canvasPos.y + m_canvasSize.y);
        
        // Draw the canvas texture
        drawCanvasTexture();
        
        // Handle mouse input
        handleMouseInput();
    }
    ImGui::End();
}

void CanvasWindow::drawCanvasTexture() {
    if (!m_resourceManager || m_activeCanvasId == entt::null) {
        return;
    }
    
    auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
    if (canvasPtr && *canvasPtr) {
        unsigned int texId = (*canvasPtr)->getColorTexture();
        printf("[ImGui] Displaying texture: ID=%u, size=%.1fx%.1f\n", texId, m_canvasSize.x, m_canvasSize.y);
        
        // Draw the image
        ImGui::Image((void*)(intptr_t)texId, m_canvasSize);
        
        // Update canvas position to match the actual image position
        m_canvasPos = ImGui::GetItemRectMin();
        m_canvasEnd = ImGui::GetItemRectMax();
        m_canvasSize = ImVec2(m_canvasEnd.x - m_canvasPos.x, m_canvasEnd.y - m_canvasPos.y);
        
        printf("[ImGui] Image bounds: pos=(%.1f,%.1f), size=(%.1f,%.1f), end=(%.1f,%.1f)\n", 
               m_canvasPos.x, m_canvasPos.y, m_canvasSize.x, m_canvasSize.y, m_canvasEnd.x, m_canvasEnd.y);
    } else {
        printf("[ImGui] ERROR: No canvas resource found for active canvas\n");
    }
}

void CanvasWindow::handleShapeCreation() {
    if (m_currentTool == 0) { // Rectangle tool
        if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
            printf("[Mouse] Left mouse clicked! canvasMousePos=(%.1f,%.1f)\n", m_canvasMousePos.x, m_canvasMousePos.y);
            m_toolStartPos = m_canvasMousePos;
            m_toolActive = true;
        }
        
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_toolActive) {
            printf("[Mouse] Left mouse released, creating shape...\n");
            printf("[Mouse] DEBUG: Tool was active, creating shape with start=(%.1f,%.1f), end=(%.1f,%.1f)\n", 
                   m_toolStartPos.x, m_toolStartPos.y, m_canvasMousePos.x, m_canvasMousePos.y);
            createShape(m_toolStartPos, m_canvasMousePos);
            m_toolActive = false;
        }
    }
}

void CanvasWindow::createShape(const ImVec2& start, const ImVec2& end) {
    if (!m_ecs) {
        printf("[CanvasWindow] ERROR: No ECS manager available\n");
        return;
    }
    
    printf("[CanvasWindow] Input coordinates: start=(%.1f,%.1f), end=(%.1f,%.1f)\n", start.x, start.y, end.x, end.y);
    
    ImVec2 blend2DStart = convertToBlend2DCoordinates(start);
    ImVec2 blend2DEnd = convertToBlend2DCoordinates(end);
    
    printf("[CanvasWindow] Converted coordinates: start=(%.1f,%.1f), end=(%.1f,%.1f)\n", blend2DStart.x, blend2DStart.y, blend2DEnd.x, blend2DEnd.y);
    
    float x1 = std::min(blend2DStart.x, blend2DEnd.x);
    float y1 = std::min(blend2DStart.y, blend2DEnd.y);
    float x2 = std::max(blend2DStart.x, blend2DEnd.x);
    float y2 = std::max(blend2DStart.y, blend2DEnd.y);
    
    printf("[CanvasWindow] Final shape coordinates: (%.1f,%.1f) to (%.1f,%.1f)\n", x1, y1, x2, y2);
    
    // Create ECS entity with shape components
    entt::entity shapeEntity = m_ecs->createEntity();
    
    // Add Transform component
    Transform transform;
    transform.x = 0.0f;
    transform.y = 0.0f;
    transform.scaleX = 1.0f;
    transform.scaleY = 1.0f;
    m_ecs->addComponent<Transform>(shapeEntity, transform);
    
    // Add Shape component
    blot::components::Shape shape;
    shape.type = blot::components::Shape::Type::Rectangle;
    shape.x1 = x1;
    shape.y1 = y1;
    shape.x2 = x2;
    shape.y2 = y2;
    m_ecs->addComponent<blot::components::Shape>(shapeEntity, shape);
    
    // Add Style component
    blot::components::Style style;
    style.setFillColor(m_fillColor.x, m_fillColor.y, m_fillColor.z, m_fillColor.w);
    style.setStrokeColor(m_strokeColor.x, m_strokeColor.y, m_strokeColor.z, m_strokeColor.w);
    style.setStrokeWidth(m_strokeWidth);
    style.hasFill = true;
    style.hasStroke = true;
    m_ecs->addComponent<blot::components::Style>(shapeEntity, style);
    
    printf("[CanvasWindow] Created shape entity: %u\n", static_cast<unsigned int>(shapeEntity));
}

ImVec2 CanvasWindow::convertToCanvasCoordinates(const ImVec2& screenPos) const {
    return ImVec2(screenPos.x - m_canvasPos.x, screenPos.y - m_canvasPos.y);
}

ImVec2 CanvasWindow::convertToBlend2DCoordinates(const ImVec2& canvasPos) const {
    printf("[CanvasWindow] convertToBlend2DCoordinates: input=(%.1f,%.1f)\n", canvasPos.x, canvasPos.y);
    
    // Get actual canvas dimensions from the canvas resource
    if (!m_resourceManager || m_activeCanvasId == entt::null) {
        printf("[CanvasWindow] ERROR: No resource manager or active canvas\n");
        return canvasPos; // Return unchanged if no canvas available
    }
    
    auto canvasPtr = m_resourceManager->getCanvas(m_activeCanvasId);
    if (!canvasPtr || !*canvasPtr) {
        printf("[CanvasWindow] ERROR: Active canvas not found in resources\n");
        return canvasPos; // Return unchanged if canvas not found
    }
    
    auto graphics = (*canvasPtr)->getGraphics();
    if (!graphics) {
        printf("[CanvasWindow] ERROR: No graphics object in canvas\n");
        return canvasPos; // Return unchanged if no graphics
    }
    
    auto renderer = graphics->getRenderer();
    if (!renderer) {
        printf("[CanvasWindow] ERROR: No renderer in graphics object\n");
        return canvasPos; // Return unchanged if no renderer
    }
    
    float canvasWidth = static_cast<float>(renderer->getWidth());
    float canvasHeight = static_cast<float>(renderer->getHeight());
    
    printf("[CanvasWindow] Canvas dimensions: %.1fx%.1f, ImGui size: %.1fx%.1f\n", canvasWidth, canvasHeight, m_canvasSize.x, m_canvasSize.y);
    
    if (m_canvasSize.x <= 0.0f || m_canvasSize.y <= 0.0f) {
        printf("[CanvasWindow] ERROR: Invalid canvas size: %.1fx%.1f\n", m_canvasSize.x, m_canvasSize.y);
        return canvasPos; // Return unchanged if invalid size
    }
    
    float scaleX = canvasWidth / m_canvasSize.x;
    float scaleY = canvasHeight / m_canvasSize.y;
    
    ImVec2 result = ImVec2(canvasPos.x * scaleX, canvasPos.y * scaleY);
    printf("[CanvasWindow] Scale: (%.2f,%.2f), result: (%.1f,%.1f)\n", scaleX, scaleY, result.x, result.y);
    
    return result;
}

} // namespace blot 