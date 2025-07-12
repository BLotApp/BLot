#pragma once

#include "Window.h"
#include <memory>

// Forward declaration
class CodeEditor;

namespace blot {

class CodeEditorWindow : public Window {
public:
    CodeEditorWindow(const std::string& title, Window::Flags flags = Window::Flags::None);
    ~CodeEditorWindow() = default;

    void setCodeEditor(std::shared_ptr<CodeEditor> codeEditor) { m_codeEditor = codeEditor; }

protected:
    void render() override;

private:
    std::shared_ptr<CodeEditor> m_codeEditor;
};

} // namespace blot 