#include "CodeEditorWindow.h"
#include <functional>
#include "TextEditor.h"
#include "imgui.h"

namespace blot {

CodeEditorWindow::CodeEditorWindow(const std::string &title,
								   Window::Flags flags)
	: Window(title, flags), m_editor(std::make_unique<TextEditor>()),
	  m_language("cpp") {

	// Initialize the editor
	m_editor->SetLanguageDefinition(
		TextEditor::LanguageDefinition::CPlusPlus());
	m_editor->SetText("// Blot Creative Coding Sketch\n\nvoid setup() {\n    "
					  "size(800, 600);\n    background(255);\n}\n\nvoid draw() "
					  "{\n    // Your code here\n}");
}

void CodeEditorWindow::setCode(const std::string &code) {
	m_currentCode = code;
	if (m_editor) {
		m_editor->SetText(code);
	}
}

std::string CodeEditorWindow::getCode() const {
	if (m_editor) {
		return m_editor->GetText();
	}
	return m_currentCode;
}

void CodeEditorWindow::setLanguage(const std::string &language) {
	m_language = language;
	if (m_editor) {
		if (language == "cpp") {
			m_editor->SetLanguageDefinition(
				TextEditor::LanguageDefinition::CPlusPlus());
		} else if (language == "glsl") {
			m_editor->SetLanguageDefinition(
				TextEditor::LanguageDefinition::GLSL());
		} else if (language == "hlsl") {
			m_editor->SetLanguageDefinition(
				TextEditor::LanguageDefinition::HLSL());
		}
	}
}

void CodeEditorWindow::setOnCodeChanged(
	std::function<void(const std::string &)> callback) {
	m_onCodeChanged = callback;
}

void CodeEditorWindow::renderContents() {
	renderToolbar();
	renderEditor();
	renderStatusBar();
}

void CodeEditorWindow::renderToolbar() {
	if (ImGui::Button("New")) {
		m_editor->SetText(
			"// New sketch\n\nvoid setup() {\n    size(800, 600);\n    "
			"background(255);\n}\n\nvoid draw() {\n    // Your code here\n}");
	}
	ImGui::SameLine();
	if (ImGui::Button("Save")) {
		// TODO: Implement save functionality
	}
	ImGui::SameLine();
	if (ImGui::Button("Run")) {
		// TODO: Implement run functionality
	}
	ImGui::Separator();
}

void CodeEditorWindow::renderEditor() {
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	m_editor->Render("CodeEditor", ImGui::GetContentRegionAvail(), false, true);

	if (m_editor->IsTextChanged()) {
		std::string newCode = m_editor->GetText();
		if (newCode != m_currentCode) {
			m_currentCode = newCode;
			if (m_onCodeChanged) {
				m_onCodeChanged(newCode);
			}
		}
	}
	ImGui::PopStyleVar();
}

void CodeEditorWindow::renderStatusBar() {
	ImGui::Separator();
	ImGui::Text("Language: %s | Lines: %d", m_language.c_str(),
				m_editor->GetTotalLines());
}

} // namespace blot
