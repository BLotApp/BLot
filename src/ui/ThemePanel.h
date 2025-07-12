#pragma once

#include "Window.h"
#include "ui/ToolbarWindow.h"
#include <imgui.h>
#include <memory>

namespace blot {

class ThemePanel : public Window {
public:
    ThemePanel(const std::string& title = "Theme Panel###ThemePanel", 
               Flags flags = Flags::None);
    virtual ~ThemePanel() = default;

    // Theme functionality
    void setToolbarWindow(std::shared_ptr<ToolbarWindow> toolbarWindow) { 
        m_toolbarWindow = toolbarWindow; 
    }
    
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
    std::shared_ptr<ToolbarWindow> m_toolbarWindow;
    
    // Theme methods
    void renderRandomThemeButton();
    void renderColorPresets();
    void renderThemeControls();
};

} // namespace blot 