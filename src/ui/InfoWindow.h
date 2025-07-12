#pragma once

#include "Window.h"
#include "CoordinateSystem.h"
#include <imgui.h>

namespace blot {

class InfoWindow : public Window {
public:
    InfoWindow(const std::string& title = "Info Window", Flags flags = Flags::None);
    virtual ~InfoWindow() = default;

    // Info window specific methods
    void setMousePos(const ImVec2& pos);
    void setMouseDelta(const ImVec2& delta);
    void setMouseClicked(bool clicked);
    void setMouseHeld(bool held);
    void setMouseDragged(bool dragged);
    void setMouseReleased(bool released);
    void setToolActive(bool active);
    void setToolStartPos(const ImVec2& pos);
    void setCurrentTool(int toolType);
    void setCoordinateSystem(int system);
    void setShowMouseCoordinates(bool show);

    // Rendering
    virtual void render() override;

    // Window interface overrides
    void show() override;
    void hide() override;
    void close() override;
    void toggle() override;
    bool isOpen() const override;
    bool isVisible() const override;
    bool isFocused() const override;
    bool isHovered() const override;
    bool isDragging() const override;
    bool isResizing() const override;
    void setPosition(const ImVec2&) override;
    void setSize(const ImVec2&) override;
    void setMinSize(const ImVec2&) override;
    void setMaxSize(const ImVec2&) override;
    void setFlags(Window::Flags) override;
    int getFlags() const override;
    std::string getTitle() const override;

private:
    // Mouse state
    ImVec2 m_mousePos = ImVec2(0, 0);
    ImVec2 m_mouseDelta = ImVec2(0, 0);
    bool m_mouseClicked = false;
    bool m_mouseHeld = false;
    bool m_mouseDragged = false;
    bool m_mouseReleased = false;
    
    // Tool state
    bool m_toolActive = false;
    ImVec2 m_toolStartPos = ImVec2(0, 0);
    int m_currentTool = 0;
    
    // Display options
    int m_coordinateSystem = 0;
    bool m_showMouseCoordinates = true;
    
    // Coordinate system names
    static const char* coordinateSystemNames[];
    
    // Helper methods
    void renderMouseInfo();
    void renderToolInfo();
    void renderCoordinateInfo();
    CoordinateSystem::CoordinateInfo getCoordinateInfo() const;
};

} // namespace blot 