#pragma once

#include <glm/glm.hpp>
#include <string>

namespace blot {

enum class CoordinateSpace {
	Screen, // Absolute screen coordinates
	App,	// Application window coordinates
	Window, // Canvas window coordinates
	Canvas	// Canvas image coordinates
};

class CoordinateSystem {
  public:
	CoordinateSystem();
	~CoordinateSystem() = default;

	// Set up coordinate spaces
	void setScreenSize(float width, float height);
	void setAppWindow(int x, int y, float width, float height);
	void setCanvasWindow(float x, float y, float width, float height);
	void setCanvasImage(float width, float height);

	// Transform coordinates between spaces
	glm::vec2 transform(glm::vec2 point, CoordinateSpace from,
						CoordinateSpace to);

	// Get coordinate info for display
	struct CoordinateInfo {
		glm::vec2 mouse;
		glm::vec2 relative;
		glm::vec2 bounds;
		std::string spaceName;
		std::string description;
	};

	CoordinateInfo getCoordinateInfo(glm::vec2 mousePos, CoordinateSpace space);

	// Utility functions
	glm::vec2 getMousePosition() const;
	glm::vec2 getScreenSize() const;
	glm::vec2 getAppWindowSize() const;
	glm::vec2 getCanvasWindowSize() const;
	glm::vec2 getCanvasImageSize() const;

  private:
	// Screen space (absolute)
	glm::vec2 m_screenSize;

	// App window space
	glm::vec2 m_appPos;
	glm::vec2 m_appSize;

	// Canvas window space
	glm::vec2 m_canvasWindowPos;
	glm::vec2 m_canvasWindowSize;

	// Canvas image space
	glm::vec2 m_canvasImageSize;

	// Helper functions
	glm::vec2 screenToApp(glm::vec2 point);
	glm::vec2 appToScreen(glm::vec2 point);
	glm::vec2 appToWindow(glm::vec2 point);
	glm::vec2 windowToApp(glm::vec2 point);
	glm::vec2 windowToCanvas(glm::vec2 point);
	glm::vec2 canvasToWindow(glm::vec2 point);
};

} // namespace blot
