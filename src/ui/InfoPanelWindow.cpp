#include "InfoPanelWindow.h"
#include "CoordinateSystem.h"
#include <imgui.h>
#include <GLFW/glfw3.h>

namespace blot {

const char* InfoPanelWindow::coordinateSystemNames[] = { "Screen", "App", "Window", "Canvas" };

InfoPanelWindow::InfoPanelWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
    // Set initial window size
    m_state.size = ImVec2(300, 400);
    m_state.position = ImVec2(10, 10);
}

void InfoPanelWindow::setMousePos(const ImVec2& pos) {
    m_mousePos = pos;
}

void InfoPanelWindow::setMouseDelta(const ImVec2& delta) {
    m_mouseDelta = delta;
}

void InfoPanelWindow::setMouseClicked(bool clicked) {
    m_mouseClicked = clicked;
}

void InfoPanelWindow::setMouseHeld(bool held) {
    m_mouseHeld = held;
}

void InfoPanelWindow::setMouseDragged(bool dragged) {
    m_mouseDragged = dragged;
}

void InfoPanelWindow::setMouseReleased(bool released) {
    m_mouseReleased = released;
}

void InfoPanelWindow::setToolActive(bool active) {
    m_toolActive = active;
}

void InfoPanelWindow::setToolStartPos(const ImVec2& pos) {
    m_toolStartPos = pos;
}

void InfoPanelWindow::setCurrentTool(int toolType) {
    m_currentTool = toolType;
}

void InfoPanelWindow::setCoordinateSystem(int system) {
    m_coordinateSystem = system;
}

void InfoPanelWindow::setShowMouseCoordinates(bool show) {
    m_showMouseCoordinates = show;
}

void InfoPanelWindow::render() {
    // Set window size and position
    ImGui::SetNextWindowSize(m_state.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(m_state.position, ImGuiCond_FirstUseEver);
    
    // Begin the window
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        renderMouseInfo();
        renderToolInfo();
        renderCoordinateInfo();
    }
    ImGui::End();
}

void InfoPanelWindow::renderMouseInfo() {
    if (ImGui::CollapsingHeader("Mouse Interaction", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Position: (%.1f, %.1f)", m_mousePos.x, m_mousePos.y);
        ImGui::Text("Delta: (%.1f, %.1f)", m_mouseDelta.x, m_mouseDelta.y);
        ImGui::Text("Clicked: %s", m_mouseClicked ? "Yes" : "No");
        ImGui::Text("Held: %s", m_mouseHeld ? "Yes" : "No");
        ImGui::Text("Dragged: %s", m_mouseDragged ? "Yes" : "No");
        ImGui::Text("Released: %s", m_mouseReleased ? "Yes" : "No");
    }
}

void InfoPanelWindow::renderToolInfo() {
    if (ImGui::CollapsingHeader("Tool State", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Active: %s", m_toolActive ? "Yes" : "No");
        ImGui::Text("Start Pos: (%.1f, %.1f)", m_toolStartPos.x, m_toolStartPos.y);
        ImGui::Text("Current Tool: %d", m_currentTool);
    }
}

void InfoPanelWindow::renderCoordinateInfo() {
    if (ImGui::CollapsingHeader("Coordinate System", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Coordinate system dropdown
        if (ImGui::Combo("System", &m_coordinateSystem, coordinateSystemNames, 4)) {
            // Coordinate system changed
        }
        
        // Get coordinate info
        CoordinateSystem::CoordinateInfo coordInfo = getCoordinateInfo();
        
        ImGui::Text("Mouse Position: (%.1f, %.1f)", coordInfo.mouse.x, coordInfo.mouse.y);
        ImGui::Text("Space: %s", coordInfo.spaceName.c_str());
        ImGui::Text("Description: %s", coordInfo.description.c_str());
    }
}

CoordinateSystem::CoordinateInfo InfoPanelWindow::getCoordinateInfo() const {
    CoordinateSystem coordSystem;
    return coordSystem.getCoordinateInfo(glm::vec2(m_mousePos.x, m_mousePos.y), static_cast<CoordinateSpace>(m_coordinateSystem));
}

} // namespace blot 