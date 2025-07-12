#include "TerminalWindow.h"
#include <imgui.h>
#include <iostream>
#include <algorithm>

namespace blot {

TerminalWindow::TerminalWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
    // Set initial window size
    m_state.size = ImVec2(600, 400);
    m_state.position = ImVec2(100, 100);
    
    // Add welcome message
    addLog("Terminal initialized. Type 'help' for available commands.");
}

// Window interface implementations
void TerminalWindow::show() { m_state.isOpen = true; }
void TerminalWindow::hide() { m_state.isOpen = false; }
void TerminalWindow::close() { m_state.isOpen = false; }
void TerminalWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool TerminalWindow::isOpen() const { return m_state.isOpen; }
bool TerminalWindow::isVisible() const { return m_state.isOpen; }
bool TerminalWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool TerminalWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool TerminalWindow::isDragging() const { return false; }
bool TerminalWindow::isResizing() const { return false; }
void TerminalWindow::setPosition(const ImVec2& pos) { m_state.position = pos; }
void TerminalWindow::setSize(const ImVec2& size) { m_state.size = size; }
void TerminalWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void TerminalWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void TerminalWindow::setFlags(Window::Flags flags) { m_state.flags = static_cast<int>(flags); }
int TerminalWindow::getFlags() const { return m_state.flags; }
std::string TerminalWindow::getTitle() const { return m_title; }

void TerminalWindow::render() {
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        renderLogHistory();
        renderInput();
    }
    ImGui::End();
}

void TerminalWindow::renderLogHistory() {
    // Create a child window for the log history
    ImGui::BeginChild("LogHistory", ImVec2(0, -ImGui::GetFrameHeightWithSpacing()), true);
    
    for (const auto& log : m_logHistory) {
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

void TerminalWindow::processCommand(const std::string& command) {
    std::string cmd = command;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "help" || cmd == "h") {
        addLog("Available commands:");
        addLog("  help, h     - Show this help");
        addLog("  clear, cls  - Clear terminal");
        addLog("  version     - Show version info");
        addLog("  echo <text> - Echo text");
    }
    else if (cmd == "clear" || cmd == "cls") {
        clearLog();
    }
    else if (cmd == "version") {
        addLog("Blot Terminal v1.0.0");
    }
    else if (cmd.substr(0, 4) == "echo") {
        std::string text = command.substr(4);
        if (!text.empty() && text[0] == ' ') {
            text = text.substr(1);
        }
        addLog(text);
    }
    else {
        addLog("Unknown command: '" + command + "'. Type 'help' for available commands.");
    }
}

void TerminalWindow::addLog(const std::string& message) {
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

void TerminalWindow::executeCommand(const std::string& command) {
    addLog("> " + command);
    processCommand(command);
}

} // namespace blot 