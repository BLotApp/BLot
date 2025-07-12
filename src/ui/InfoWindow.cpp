#include "InfoWindow.h"
#include "CoordinateSystem.h"
#include <imgui.h>

namespace blot {

const char* InfoWindow::coordinateSystemNames[] = { "Current_Screen", "Main_Window", "Current_Window", "Current_Canvas" };

InfoWindow::InfoWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
    // Set initial window size
    m_state.size = ImVec2(300, 200);
    m_state.position = ImVec2(10, 10);
}

// Window interface implementations
void InfoWindow::show() { m_state.isOpen = true; }
void InfoWindow::hide() { m_state.isOpen = false; }
void InfoWindow::close() { m_state.isOpen = false; }
void InfoWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool InfoWindow::isOpen() const { return m_state.isOpen; }
bool InfoWindow::isVisible() const { return m_state.isOpen; }
bool InfoWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool InfoWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool InfoWindow::isDragging() const { return false; }
bool InfoWindow::isResizing() const { return false; }
void InfoWindow::setPosition(const ImVec2& pos) { m_state.position = pos; }
void InfoWindow::setSize(const ImVec2& size) { m_state.size = size; }
void InfoWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void InfoWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void InfoWindow::setFlags(Window::Flags flags) { m_state.flags = static_cast<int>(flags); }
int InfoWindow::getFlags() const { return m_state.flags; }
std::string InfoWindow::getTitle() const { return m_title; }

void InfoWindow::setMousePos(const ImVec2& pos) {
    m_mousePos = pos;
}

void InfoWindow::setMouseDelta(const ImVec2& delta) {
    m_mouseDelta = delta;
}

void InfoWindow::setMouseClicked(bool clicked) {
    m_mouseClicked = clicked;
}

void InfoWindow::setMouseHeld(bool held) {
    m_mouseHeld = held;
}

void InfoWindow::setMouseDragged(bool dragged) {
    m_mouseDragged = dragged;
}

void InfoWindow::setMouseReleased(bool released) {
    m_mouseReleased = released;
}

void InfoWindow::setToolActive(bool active) {
    m_toolActive = active;
}

void InfoWindow::setToolStartPos(const ImVec2& pos) {
    m_toolStartPos = pos;
}

void InfoWindow::setCurrentTool(int toolType) {
    m_currentTool = toolType;
}

void InfoWindow::setCoordinateSystem(int system) {
    m_coordinateSystem = system;
}

void InfoWindow::setShowMouseCoordinates(bool show) {
    m_showMouseCoordinates = show;
}

void InfoWindow::render() {
    // Set window size and position
    ImGui::SetNextWindowSize(m_state.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(m_state.position, ImGuiCond_FirstUseEver);
    
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        renderMouseInfo();
        renderToolInfo();
        renderCoordinateInfo();
    }
    ImGui::End();
}

void InfoWindow::renderMouseInfo() {
    if (ImGui::CollapsingHeader("Mouse Interaction", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Position: (%.1f, %.1f)", m_mousePos.x, m_mousePos.y);
        ImGui::Text("Delta: (%.1f, %.1f)", m_mouseDelta.x, m_mouseDelta.y);
        ImGui::Text("Clicked: %s", m_mouseClicked ? "Yes" : "No");
        ImGui::Text("Held: %s", m_mouseHeld ? "Yes" : "No");
        ImGui::Text("Dragged: %s", m_mouseDragged ? "Yes" : "No");
        ImGui::Text("Released: %s", m_mouseReleased ? "Yes" : "No");
    }
}

void InfoWindow::renderToolInfo() {
    if (ImGui::CollapsingHeader("Tool State", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Active: %s", m_toolActive ? "Yes" : "No");
        ImGui::Text("Start Position: (%.1f, %.1f)", m_toolStartPos.x, m_toolStartPos.y);
        ImGui::Text("Current Tool: %d", m_currentTool);
    }
}

void InfoWindow::renderCoordinateInfo() {
    if (ImGui::CollapsingHeader("Coordinate System", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Coordinate system dropdown
        if (ImGui::Combo("System", &m_coordinateSystem, coordinateSystemNames, 4)) {
            // Handle coordinate system change
        }
        
        // Show mouse coordinates checkbox
        ImGui::Checkbox("Show Mouse Coordinates", &m_showMouseCoordinates);
        
        if (m_showMouseCoordinates) {
            auto coordInfo = getCoordinateInfo();
            ImGui::Text("Mouse: (%.1f, %.1f)", coordInfo.mouse.x, coordInfo.mouse.y);
            ImGui::Text("Relative: (%.1f, %.1f)", coordInfo.relative.x, coordInfo.relative.y);
            ImGui::Text("Bounds: (%.1f, %.1f)", coordInfo.bounds.x, coordInfo.bounds.y);
            ImGui::Text("Space: %s", coordInfo.spaceName.c_str());
            ImGui::Text("Description: %s", coordInfo.description.c_str());
        }
    }
}

CoordinateSystem::CoordinateInfo InfoWindow::getCoordinateInfo() const {
    CoordinateSystem coordSystem;
    return coordSystem.getCoordinateInfo(glm::vec2(m_mousePos.x, m_mousePos.y), static_cast<CoordinateSpace>(m_coordinateSystem));
}

} // namespace blot 