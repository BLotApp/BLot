#include "ImGuiWindow.h"
#include <imgui.h>

namespace blot {

ImGuiWindow::ImGuiWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
}

void ImGuiWindow::render() {
    if (m_isOpen) {
        // Begin the window
        bool open = m_isOpen;
        if (ImGui::Begin(m_title.c_str(), &open, m_flags)) {
            if (m_renderCallback) {
                m_renderCallback();
            } else {
                // Default rendering - subclasses should override this
                ImGui::Text("ImGuiWindow base class - override render() in derived class");
            }
        }
        ImGui::End();
        
        // Update open state
        m_isOpen = open;
    }
}

void ImGuiWindow::setRenderCallback(std::function<void()> callback) {
    m_renderCallback = callback;
}

} // namespace blot 