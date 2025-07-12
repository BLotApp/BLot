#include "CodeEditorWindow.h"
#include "CodeEditor.h"
#include "imgui.h"

namespace blot {

CodeEditorWindow::CodeEditorWindow(const std::string& title, Window::Flags flags) 
    : Window(title, flags) {
}

void CodeEditorWindow::render() {
    if (m_codeEditor) {
        m_codeEditor->render();
    }
}

} // namespace blot 