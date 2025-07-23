#include "MShortcut.h"
#include <imgui.h>
#include "rendering/U_gladGlfw.h"

void MShortcut::registerShortcut(ImGuiKey key, int modifiers,
								 std::function<void()> callback,
								 const std::string &description) {
	m_shortcuts.push_back({key, modifiers, std::move(callback), description});
}

int MShortcut::getCurrentModifiers() {
	int mods = 0;
	ImGuiIO &io = ImGui::GetIO();
	if (io.KeyCtrl)
		mods |= Mod_Ctrl;
	if (io.KeyShift)
		mods |= Mod_Shift;
	if (io.KeyAlt)
		mods |= Mod_Alt;
	if (io.KeySuper)
		mods |= Mod_Super;
	return mods;
}

void MShortcut::processShortcuts() {
	ImGuiIO &io = ImGui::GetIO();
	if (io.WantCaptureKeyboard)
		return;

	// F1 toggles help overlay
	static bool f1Pressed = false;
	if (ImGui::IsKeyPressed(ImGuiKey_F1)) {
		if (!f1Pressed) {
			m_showHelp = !m_showHelp;
			f1Pressed = true;
		}
	} else {
		f1Pressed = false;
	}

	int currentMods = getCurrentModifiers();
	for (const auto &shortcut : m_shortcuts) {
		if (ImGui::IsKeyPressed(shortcut.key, false) &&
			(getCurrentModifiers() == shortcut.modifiers)) {
			shortcut.callback();
		}
	}
}

void MShortcut::showHelpOverlay() {
	if (!m_showHelp)
		return;
	ImGui::Begin("Shortcuts Help", &m_showHelp,
				 ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Keyboard Shortcuts:");
	ImGui::Separator();
	for (const auto &shortcut : m_shortcuts) {
		std::string modStr;
		if (shortcut.modifiers & Mod_Ctrl)
			modStr += "Ctrl+";
		if (shortcut.modifiers & Mod_Shift)
			modStr += "Shift+";
		if (shortcut.modifiers & Mod_Alt)
			modStr += "Alt+";
		if (shortcut.modifiers & Mod_Super)
			modStr += "Super+";
		ImGui::BulletText("%s%s: %s", modStr.c_str(),
						  glfwGetKeyName(shortcut.key, 0),
						  shortcut.description.c_str());
	}
	ImGui::End();
}
