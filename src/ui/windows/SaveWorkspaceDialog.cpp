#include "SaveWorkspaceDialog.h"
#include <cstring>
#include "imgui.h"

namespace blot {

SaveWorkspaceDialog::SaveWorkspaceDialog(const std::string &title,
										 Window::Flags flags)
	: Window(title, flags) {
	// Initialize the name buffer with the initial workspace name
	strncpy_s(m_nameBuffer, m_workspaceName.c_str(), sizeof(m_nameBuffer) - 1);
	m_nameBuffer[sizeof(m_nameBuffer) - 1] = '\0';
}

void SaveWorkspaceDialog::renderContents() {
	// Set a reasonable minimum size every frame to prevent tiny/narrow dialogs
	ImGui::SetNextWindowSize(ImVec2(400, 220), ImGuiCond_Appearing);
	ImGui::SetWindowSize(ImVec2(400, 220), ImGuiCond_Always);

	// Make the main content scrollable if needed
	ImGui::BeginChild("##DialogContent", ImVec2(0, 100), false,
					  ImGuiWindowFlags_AlwaysVerticalScrollbar);
	ImGui::Text("Enter workspace name:");
	ImGui::Spacing();
	// Focus on the input field when the dialog opens
	if (ImGui::IsWindowAppearing()) {
		ImGui::SetKeyboardFocusHere();
	}
	// Use a visible label for the input
	bool enterPressed =
		ImGui::InputText("Workspace Name", m_nameBuffer, sizeof(m_nameBuffer),
						 ImGuiInputTextFlags_EnterReturnsTrue);
	ImGui::EndChild();

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	// Buttons
	float buttonWidth = 80.0f;
	float spacing = 10.0f;
	float totalWidth = buttonWidth * 2 + spacing;
	float startX = (ImGui::GetWindowWidth() - totalWidth) * 0.5f;
	ImGui::SetCursorPosX(startX);
	bool saveClicked = ImGui::Button("Save", ImVec2(buttonWidth, 0));
	ImGui::SameLine();
	ImGui::Dummy(ImVec2(spacing, 0));
	ImGui::SameLine();
	bool cancelClicked = ImGui::Button("Cancel", ImVec2(buttonWidth, 0));
	// Save on Enter or Save button
	if ((enterPressed || saveClicked) && m_saveCallback &&
		strlen(m_nameBuffer) > 0) {
		m_saveCallback(m_nameBuffer);
		m_shouldClose = true;
	}
	if (cancelClicked) {
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
