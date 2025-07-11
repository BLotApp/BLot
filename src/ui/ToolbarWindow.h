#pragma once

#include "Window.h"
#include <imgui.h>
#include <functional>

namespace blot {

class ToolbarWindow : public Window {
public:
    ToolbarWindow(const std::string& title = "Toolbar", Flags flags = Flags::None);
    virtual ~ToolbarWindow() = default;

    // Toolbar specific methods
    void setCurrentTool(int toolType);
    void setFillColor(const ImVec4& color);
    void setStrokeColor(const ImVec4& color);
    void setStrokeWidth(float width);
    void setShowSwatches(bool show);
    void setShowStrokePalette(bool show);
    
    // Callbacks
    void setOnToolChanged(std::function<void(int)> callback);
    void setOnFillColorChanged(std::function<void(const ImVec4&)> callback);
    void setOnStrokeColorChanged(std::function<void(const ImVec4&)> callback);
    void setOnStrokeWidthChanged(std::function<void(float)> callback);

    // Rendering
    virtual void render() override;

private:
    // Tool state
    int m_currentTool = 0;
    ImVec4 m_fillColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 m_strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_strokeWidth = 2.0f;
    
    // Display options
    bool m_showSwatches = false;
    bool m_showStrokePalette = false;
    
    // Callbacks
    std::function<void(int)> m_onToolChanged;
    std::function<void(const ImVec4&)> m_onFillColorChanged;
    std::function<void(const ImVec4&)> m_onStrokeColorChanged;
    std::function<void(float)> m_onStrokeWidthChanged;
    
    // Helper methods
    void renderTools();
    void renderColors();
    void renderSwatches();
    void renderStrokePalette();
    void toolButton(const char* icon, int toolType);
};

} // namespace blot 