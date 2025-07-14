#include "SaveWorkspaceDialog.h"
#include "imgui.h"
#include <cstring>

namespace blot {

SaveWorkspaceDialog::SaveWorkspaceDialog(const std::string& title, Window::Flags flags) 
    : Window(title, flags) {
    // Initialize the name buffer with the initial workspace name
    strncpy_s(m_nameBuffer, m_workspaceName.c_str(), sizeof(m_nameBuffer) - 1);
    m_nameBuffer[sizeof(m_nameBuffer) - 1] = '\0';
}

void SaveWorkspaceDialog::renderContents() {
    ImGui::SetNextWindowSize(ImVec2(400, 150), ImGuiCond_FirstUseEver);
    ImGui::Text("Enter workspace name:");
    ImGui::Spacing();
    // Focus on the input field when the dialog opens
    if (ImGui::IsWindowAppearing()) {
        ImGui::SetKeyboardFocusHere();
    }
    if (ImGui::InputText("##WorkspaceName", m_nameBuffer, sizeof(m_nameBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        // Save when Enter is pressed
        if (m_saveCallback && strlen(m_nameBuffer) > 0) {
            m_saveCallback(m_nameBuffer);
            m_shouldClose = true;
        }
    }
    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();
    // Buttons
    float buttonWidth = 80.0f;
    float spacing = 10.0f;
    float totalWidth = buttonWidth * 2 + spacing;
    float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;
    ImGui::SetCursorPosX(startX);
    if (ImGui::Button("Save", ImVec2(buttonWidth, 0))) {
        if (m_saveCallback && strlen(m_nameBuffer) > 0) {
            m_saveCallback(m_nameBuffer);
            m_shouldClose = true;
        }
    }
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(spacing, 0));
    ImGui::SameLine();
    if (ImGui::Button("Cancel", ImVec2(buttonWidth, 0))) {
        if (m_cancelCallback) {
            m_cancelCallback();
        }
        m_shouldClose = true;
    }
    // Close the dialog if requested
    if (m_shouldClose) {
        m_isOpen = false;
        m_shouldClose = false;
    }
}

} // namespace blot 