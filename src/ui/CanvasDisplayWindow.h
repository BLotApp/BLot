#pragma once

#include "ImGuiWindow.h"
#include <memory>

namespace blot {

class CanvasDisplayWindow : public ImGuiWindow {
public:
    CanvasDisplayWindow(const std::string& title = "Canvas###MainCanvas", 
                       Flags flags = Flags::NoScrollbar | Flags::NoCollapse);
    virtual ~CanvasDisplayWindow() = default;

    // Canvas display methods
    void setCanvasTexture(unsigned int textureId, int width, int height);
    void clearCanvasTexture();
    
    // Mouse interaction
    void handleMouseInput();
    ImVec2 getCanvasMousePos() const;
    bool isMouseInsideCanvas() const;
    ImVec2 getCanvasSize() const;
    
    // Window identification
    virtual std::string getName() const override;
    virtual std::string getTitle() const override;

    // Rendering
    virtual void render() override;

private:
    // Canvas texture
    unsigned int m_canvasTextureId = 0;
    int m_canvasWidth = 0;
    int m_canvasHeight = 0;
    bool m_hasTexture = false;
    
    // Mouse state
    ImVec2 m_canvasPos = ImVec2(0, 0);
    ImVec2 m_canvasSize = ImVec2(0, 0);
    ImVec2 m_canvasEnd = ImVec2(0, 0);
    ImVec2 m_canvasMousePos = ImVec2(0, 0);
    bool m_mouseInsideCanvas = false;
    
    // Helper methods
    void drawCanvasTexture();
    ImVec2 convertToCanvasCoordinates(const ImVec2& screenPos) const;
};

} // namespace blot 