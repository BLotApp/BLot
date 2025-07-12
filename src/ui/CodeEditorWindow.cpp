#include "CodeEditorWindow.h"
#include "CodeEditor.h"
#include "imgui.h"

namespace blot {

CodeEditorWindow::CodeEditorWindow(const std::string& title, Window::Flags flags) 
    : Window(title, flags) {
}

// Window interface implementations
void CodeEditorWindow::show() { m_state.isOpen = true; }
void CodeEditorWindow::hide() { m_state.isOpen = false; }
void CodeEditorWindow::close() { m_state.isOpen = false; }
void CodeEditorWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool CodeEditorWindow::isOpen() const { return m_state.isOpen; }
bool CodeEditorWindow::isVisible() const { return m_state.isOpen; }
bool CodeEditorWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool CodeEditorWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool CodeEditorWindow::isDragging() const { return false; }
bool CodeEditorWindow::isResizing() const { return false; }
void CodeEditorWindow::setPosition(const ImVec2& pos) { /* TODO: Implement if needed */ }
void CodeEditorWindow::setSize(const ImVec2& size) { /* TODO: Implement if needed */ }
void CodeEditorWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void CodeEditorWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void CodeEditorWindow::setFlags(Window::Flags flags) { m_state.flags = flags; }
Window::Flags CodeEditorWindow::getFlags() const { return m_state.flags; }
const std::string& CodeEditorWindow::getTitle() const { return m_title; }
void CodeEditorWindow::setTitle(const std::string& title) { m_title = title; }

void CodeEditorWindow::render() {
    if (m_codeEditor) {
        m_codeEditor->render();
    }
}

} // namespace blot 