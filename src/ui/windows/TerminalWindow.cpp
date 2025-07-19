#include "TerminalWindow.h"
#include <algorithm>
#include <imgui.h>
#include <iostream>

namespace blot {

TerminalWindow::TerminalWindow(const std::string &title, Flags flags)
	: Window(title, flags) {
	// Add welcome message
	addLog("Terminal initialized. Type 'help' for available commands.");
}

void TerminalWindow::renderContents() {
	renderLogHistory();
	renderInput();
}

void TerminalWindow::renderLogHistory() {
	// Create a child window for the log history
	ImGui::BeginChild("LogHistory",
					  ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);

	for (const auto &log : m_logHistory) {
		ImGui::TextWrapped("%s", log.c_str());
	}

	if (m_scrollToBottom) {
		ImGui::SetScrollHereY(1.0f);
		m_scrollToBottom = false;
	}

	ImGui::EndChild();
}

void TerminalWindow::renderInput() {
	ImGui::PushItemWidth(-1);
	if (ImGui::InputText("##Command", m_inputBuffer, sizeof(m_inputBuffer),
						 ImGuiInputTextFlags_EnterReturnsTrue)) {
		std::string command = m_inputBuffer;
		if (!command.empty()) {
			addLog("> " + command);
			processCommand(command);
			m_inputBuffer[0] = '\0'; // Clear input
			m_scrollToBottom = true;
		}
	}
	ImGui::PopItemWidth();

	// Auto-focus on input
	if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive()) {
		ImGui::SetKeyboardFocusHere(-1);
	}
}

void TerminalWindow::processCommand(const std::string &command) {
	std::string cmd = command;
	std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);

	if (cmd == "help" || cmd == "h") {
		addLog("Available commands:");
		addLog("  help, h     - Show this help");
		addLog("  clear, cls  - Clear terminal");
		addLog("  version     - Show version info");
		addLog("  echo <text> - Echo text");
	} else if (cmd == "clear" || cmd == "cls") {
		clearLog();
	} else if (cmd == "version") {
		addLog("Blot Terminal v1.0.0");
	} else if (cmd.substr(0, 4) == "echo") {
		std::string text = command.substr(4);
		if (!text.empty() && text[0] == ' ') {
			text = text.substr(1);
		}
		addLog(text);
	} else {
		addLog("Unknown command: '" + command +
			   "'. Type 'help' for available commands.");
	}
}

void TerminalWindow::addLog(const std::string &message) {
	m_logHistory.push_back(message);

	// Limit log history to prevent memory issues
	if (m_logHistory.size() > 1000) {
		m_logHistory.erase(m_logHistory.begin());
	}
}

void TerminalWindow::clearLog() {
	m_logHistory.clear();
	addLog("Terminal cleared.");
}

void TerminalWindow::executeCommand(const std::string &command) {
	addLog("> " + command);
	processCommand(command);
}

} // namespace blot
