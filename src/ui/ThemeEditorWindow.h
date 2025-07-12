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
    std::string* m_lastThemePath = nullptr;
    int* m_currentTheme = nullptr;
    void renderThemeEditor();
};

} // namespace blot 