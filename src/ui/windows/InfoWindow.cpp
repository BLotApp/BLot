#include "InfoWindow.h"
#include <imgui.h>
#include "CoordinateSystem.h"
#include "Mui.h"
#include "ecs/components/CTransform.h"

namespace blot {

const char *InfoWindow::coordinateSystemNames[] = {
	"Current_Screen", "Main_Window", "Current_Window", "Current_Canvas"};

InfoWindow::InfoWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

void InfoWindow::setMousePos(const ImVec2 &pos) { m_mousePos = pos; }

void InfoWindow::setMouseDelta(const ImVec2 &delta) { m_mouseDelta = delta; }

void InfoWindow::setMouseClicked(bool clicked) { m_mouseClicked = clicked; }

void InfoWindow::setMouseHeld(bool held) { m_mouseHeld = held; }

void InfoWindow::setMouseDragged(bool dragged) { m_mouseDragged = dragged; }

void InfoWindow::setMouseReleased(bool released) { m_mouseReleased = released; }

void InfoWindow::setToolActive(bool active) { m_toolActive = active; }

void InfoWindow::setToolStartPos(const ImVec2 &pos) { m_toolStartPos = pos; }

void InfoWindow::setCurrentTool(int toolType) { m_currentTool = toolType; }

void InfoWindow::setCoordinateSystem(int system) {
	m_coordinateSystem = system;
}

void InfoWindow::setShowMouseCoordinates(bool show) {
	m_showMouseCoordinates = show;
}

void InfoWindow::renderContents() {
	renderMouseInfo();
	renderToolInfo();
	renderCoordinateInfo();
}

void InfoWindow::renderMouseInfo() {
	if (ImGui::CollapsingHeader("Mouse Interaction",
								ImGuiTreeNodeFlags_DefaultOpen)) {
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
		ImGui::Text("Start Position: (%.1f, %.1f)", m_toolStartPos.x,
					m_toolStartPos.y);
		ImGui::Text("Current Tool: %d", m_currentTool);
	}
}

void InfoWindow::renderCoordinateInfo() {
	if (ImGui::CollapsingHeader("Coordinate System",
								ImGuiTreeNodeFlags_DefaultOpen)) {
		// Coordinate system dropdown
		if (ImGui::Combo("System", &m_coordinateSystem, coordinateSystemNames,
						 4)) {
			// Handle coordinate system change
		}

		// Show mouse coordinates checkbox
		ImGui::Checkbox("Show Mouse Coordinates", &m_showMouseCoordinates);

		if (m_showMouseCoordinates) {
			auto coordInfo = getCoordinateInfo();
			ImGui::Text("Mouse: (%.1f, %.1f)", coordInfo.mouse.x,
						coordInfo.mouse.y);
			ImGui::Text("Relative: (%.1f, %.1f)", coordInfo.relative.x,
						coordInfo.relative.y);
			ImGui::Text("Bounds: (%.1f, %.1f)", coordInfo.bounds.x,
						coordInfo.bounds.y);
			ImGui::Text("Space: %s", coordInfo.spaceName.c_str());
			ImGui::Text("Description: %s", coordInfo.description.c_str());
		}
	}
}

CoordinateSystem::CoordinateInfo InfoWindow::getCoordinateInfo() const {
	// TODO: Pass a pointer to Mui to InfoWindow and use it here instead
	// of getInstance Example: if (!m_uiManager) return {}; return
	// m_uiManager->getCoordinateSystem().getCoordinateInfo(glm::vec2(m_mousePos.x,
	// m_mousePos.y), static_cast<CoordinateSpace>(m_coordinateSystem)); For
	// now, return an empty CoordinateInfo
	return {};
}

} // namespace blot
