#pragma once

#include "Window.h"
#include <memory>
#include <string>
#include <functional>

// Forward declaration
class TextEditor;

namespace blot {

class CodeEditorWindow : public Window {
public:
    CodeEditorWindow(const std::string& title, Window::Flags flags = Window::Flags::None);
    ~CodeEditorWindow() = default;

    void setCode(const std::string& code);
    std::string getCode() const;
    void setLanguage(const std::string& language);
    void setOnCodeChanged(std::function<void(const std::string&)> callback);
    
    // Code editor theme (not global ImGui theme)
    void setEditorTheme(int theme) { m_editorTheme = theme; }
    int getEditorTheme() const { return m_editorTheme; }
    
    // Global theme state (for synchronization with UIManager)
    void setCurrentTheme(int theme) { m_currentTheme = theme; }
    int getCurrentTheme() const { return m_currentTheme; }

    void renderContents() override;

private:
    std::unique_ptr<TextEditor> m_editor;
    std::string m_currentCode;
    std::string m_language;
    std::function<void(const std::string&)> m_onCodeChanged;
    
    // Theme state
    int m_currentTheme = 0;
    int m_editorTheme = 0; // 0 = default, 1 = Monokai, etc.
    
    void renderToolbar();
    void renderEditor();
    void renderStatusBar();
};

} // namespace blot 