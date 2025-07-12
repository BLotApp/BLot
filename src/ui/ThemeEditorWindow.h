#pragma once

#include "Window.h"
#include <memory>
#include <string>

namespace blot {

class ThemeEditorWindow : public Window {
public:
    ThemeEditorWindow(const std::string& title = "Theme Editor", Flags flags = Flags::None);
    virtual ~ThemeEditorWindow() = default;

    void setThemePath(std::string* lastThemePath);
    void setCurrentTheme(int* currentTheme);
    virtual void render() override;

private:
    std::string* m_lastThemePath = nullptr;
    int* m_currentTheme = nullptr;
    void renderThemeEditor();
};

} // namespace blot 