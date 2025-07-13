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

protected:
    void render() override;

private:
    std::unique_ptr<TextEditor> m_editor;
    std::string m_currentCode;
    std::string m_language;
    std::function<void(const std::string&)> m_onCodeChanged;
    
    void renderToolbar();
    void renderEditor();
    void renderStatusBar();
};

} // namespace blot 