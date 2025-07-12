#include "CanvasDisplayWindow.h"
#include "imgui.h"

namespace blot {

CanvasDisplayWindow::CanvasDisplayWindow(const std::string& title, Flags flags)
    : ImGuiWindow(title, flags) {
}

void CanvasDisplayWindow::setCanvasTexture(unsigned int textureId, int width, int height) {
    m_canvasTextureId = textureId;
    m_canvasWidth = width;
    m_canvasHeight = height;
    m_hasTexture = (textureId != 0);
}

void CanvasDisplayWindow::clearCanvasTexture() {
    m_canvasTextureId = 0;
    m_canvasWidth = 0;
    m_canvasHeight = 0;
    m_hasTexture = false;
}

void CanvasDisplayWindow::handleMouseInput() {
    if (!isVisible() || !isOpen()) {
        return;
    }
    
    // Get canvas bounds
    ImVec2 windowPos = getPosition();
    ImVec2 windowSize = getSize();
    
    // Calculate canvas area (full window content area)
    m_canvasPos = ImGui::GetCursorScreenPos();
    m_canvasSize = ImGui::GetContentRegionAvail();
    m_canvasEnd = ImVec2(m_canvasPos.x + m_canvasSize.x, m_canvasPos.y + m_canvasSize.y);
    
    // Check if mouse is inside canvas
    ImVec2 mousePos = ImGui::GetMousePos();
    m_mouseInsideCanvas = (mousePos.x >= m_canvasPos.x && mousePos.x <= m_canvasEnd.x &&
                          mousePos.y >= m_canvasPos.y && mousePos.y <= m_canvasEnd.y);
    
    if (m_mouseInsideCanvas) {
        m_canvasMousePos = convertToCanvasCoordinates(mousePos);
    }
}

ImVec2 CanvasDisplayWindow::getCanvasMousePos() const {
    return m_canvasMousePos;
}

bool CanvasDisplayWindow::isMouseInsideCanvas() const {
    return m_mouseInsideCanvas;
}

ImVec2 CanvasDisplayWindow::getCanvasSize() const {
    return m_canvasSize;
}

std::string CanvasDisplayWindow::getName() const {
    return m_title;
}

std::string CanvasDisplayWindow::getTitle() const {
    return m_title;
}

void CanvasDisplayWindow::render() {
    begin();
    if (isOpen()) {
        // Display the canvas texture
        drawCanvasTexture();
        
        // Handle mouse input
        handleMouseInput();
        
        end();
    }
}

void CanvasDisplayWindow::drawCanvasTexture() {
    if (m_hasTexture && m_canvasTextureId != 0) {
        // Get available space
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        
        // Calculate aspect ratio to maintain proportions
        float aspectRatio = static_cast<float>(m_canvasWidth) / static_cast<float>(m_canvasHeight);
        float displayWidth = availableSize.x;
        float displayHeight = availableSize.x / aspectRatio;
        
        if (displayHeight > availableSize.y) {
            displayHeight = availableSize.y;
            displayWidth = availableSize.y * aspectRatio;
        }
        
        ImVec2 displaySize(displayWidth, displayHeight);
        
        // Center the image
        ImVec2 imagePos = ImGui::GetCursorScreenPos();
        ImVec2 windowSize = ImGui::GetWindowSize();
        ImVec2 windowPos = ImGui::GetWindowPos();
        
        float xOffset = (availableSize.x - displaySize.x) * 0.5f;
        float yOffset = (availableSize.y - displaySize.y) * 0.5f;
        
        ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + xOffset, ImGui::GetCursorPos().y + yOffset));
        
        // Display the texture
        ImGui::Image(reinterpret_cast<void*>(static_cast<uintptr_t>(m_canvasTextureId)), 
                    displaySize, ImVec2(0, 0), ImVec2(1, 1));
        
        // Debug info
        if (ImGui::IsWindowHovered()) {
            ImGui::SetTooltip("Canvas Texture: %dx%d", m_canvasWidth, m_canvasHeight);
        }
    } else {
        // Show placeholder when no texture
        ImVec2 availableSize = ImGui::GetContentRegionAvail();
        ImVec2 centerPos = ImVec2(ImGui::GetCursorPos().x + availableSize.x * 0.5f - 50,
                                 ImGui::GetCursorPos().y + availableSize.y * 0.5f - 10);
        
        ImGui::SetCursorPos(centerPos);
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No Canvas Texture");
    }
}

ImVec2 CanvasDisplayWindow::convertToCanvasCoordinates(const ImVec2& screenPos) const {
    if (!m_hasTexture || m_canvasWidth == 0 || m_canvasHeight == 0) {
        return ImVec2(0, 0);
    }
    
    // Convert screen coordinates to canvas coordinates
    float x = (screenPos.x - m_canvasPos.x) / m_canvasSize.x * m_canvasWidth;
    float y = (screenPos.y - m_canvasPos.y) / m_canvasSize.y * m_canvasHeight;
    
    return ImVec2(x, y);
}

} // namespace blot 