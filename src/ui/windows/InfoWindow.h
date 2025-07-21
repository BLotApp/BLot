#pragma once

#include <imgui.h>
#include "CoordinateSystem.h"
#include "Window.h"

namespace blot {
class Mui;

class InfoWindow : public Window {
  public:
	InfoWindow(const std::string &title = "Info", Flags flags = Flags::None);
	virtual ~InfoWindow() = default;

	// Info window specific methods
	void setMousePos(const ImVec2 &pos);
	void setMouseDelta(const ImVec2 &delta);
	void setMouseClicked(bool clicked);
	void setMouseHeld(bool held);
	void setMouseDragged(bool dragged);
	void setMouseReleased(bool released);
	void setToolActive(bool active);
	void setToolStartPos(const ImVec2 &pos);
	void setCurrentTool(int toolType);
	void setCoordinateSystem(int system);
	void setShowMouseCoordinates(bool show);
	void renderContents() override;

  private:
	// Mouse state
	ImVec2 m_mousePos = ImVec2(0, 0);
	ImVec2 m_mouseDelta = ImVec2(0, 0);
	bool m_mouseClicked = false;
	bool m_mouseHeld = false;
	bool m_mouseDragged = false;
	bool m_mouseReleased = false;

	// Tool state
	bool m_toolActive = false;
	ImVec2 m_toolStartPos = ImVec2(0, 0);
	int m_currentTool = 0;

	// Display options
	int m_coordinateSystem = 0;
	bool m_showMouseCoordinates = true;

	// Coordinate system names
	static const char *coordinateSystemNames[];

	// Helper methods
	void renderMouseInfo();
	void renderToolInfo();
	void renderCoordinateInfo();
	CoordinateSystem::CoordinateInfo getCoordinateInfo() const;
};

} // namespace blot
