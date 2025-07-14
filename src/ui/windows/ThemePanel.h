#pragma once

#include "Window.h"
#include "ToolbarWindow.h"
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

    void renderContents() override;

private:
    std::shared_ptr<ToolbarWindow> m_toolbarWindow;
    
    // Theme methods
    void renderRandomThemeButton();
    void renderColorPresets();
    void renderThemeControls();
};

} // namespace blot 