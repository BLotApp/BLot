#pragma once

#include "Window.h"
#include <imgui.h>
#include <functional>
#include <vector>
#include <string>

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
    
    // Tool state management
    void setToolActive(bool active);
    void setToolStartPos(const ImVec2& pos);
    bool isToolActive() const { return m_toolActive; }
    ImVec2 getToolStartPos() const { return m_toolStartPos; }
    int getCurrentTool() const { return m_currentTool; }
    
    // Swatches system
    void setSwatches(const std::vector<ImVec4>& swatches);
    void addSwatch(const ImVec4& color);
    void removeSwatch(int index);
    void saveSwatches(const std::string& path);
    void loadSwatches(const std::string& path);
    
    // Stroke controls (basic - detailed controls in StrokeWindow)
    float getStrokeWidth() const { return m_strokeWidth; }
    
    // Callbacks
    void setOnToolChanged(std::function<void(int)> callback);
    void setOnFillColorChanged(std::function<void(const ImVec4&)> callback);
    void setOnStrokeColorChanged(std::function<void(const ImVec4&)> callback);
    void setOnStrokeWidthChanged(std::function<void(float)> callback);
    void setOnToolStateChanged(std::function<void(bool, const ImVec2&)> callback);
    
    // Getters for colors
    ImVec4 getFillColor() const { return m_fillColor; }
    ImVec4 getStrokeColor() const { return m_strokeColor; }

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
    // Tool state
    int m_currentTool = 0;
    bool m_toolActive = false;
    ImVec2 m_toolStartPos = ImVec2(0, 0);
    
    // Colors
    ImVec4 m_fillColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
    ImVec4 m_strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    float m_strokeWidth = 2.0f;
    
    // Display options
    bool m_showSwatches = false;
    bool m_showStrokePalette = false;
    
    // Swatches system
    std::vector<ImVec4> m_swatches = {
        ImVec4(0,0,0,1), ImVec4(1,1,1,1), ImVec4(1,0,0,1), 
        ImVec4(0,1,0,1), ImVec4(0,0,1,1), ImVec4(1,1,0,1),
        ImVec4(1,0,1,1), ImVec4(0,1,1,1), ImVec4(0.5f,0.5f,0.5f,1)
    };
    int m_activeSwatchType = 0; // 0 = fill, 1 = stroke
    std::string m_swatchesFile = "swatches.json";
    
    // Callbacks
    std::function<void(int)> m_onToolChanged;
    std::function<void(const ImVec4&)> m_onFillColorChanged;
    std::function<void(const ImVec4&)> m_onStrokeColorChanged;
    std::function<void(float)> m_onStrokeWidthChanged;
    std::function<void(bool, const ImVec2&)> m_onToolStateChanged;
    
    // Helper methods
    void renderTools();
    void renderColors();
    void renderSwatches();
    void renderStrokePalette();
    void renderStrokeColorButtons();
    void toolButton(const char* icon, int toolType);
    void saveSwatchesToFile(const std::string& path);
    void loadSwatchesFromFile(const std::string& path);
};

} // namespace blot 