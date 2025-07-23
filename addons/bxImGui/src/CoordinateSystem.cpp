#include "CoordinateSystem.h"
#include <iomanip>
#include <sstream>

namespace blot {

CoordinateSystem::CoordinateSystem()
	: m_screenSize(0, 0), m_appPos(0, 0), m_appSize(0, 0),
	  m_canvasWindowPos(0, 0), m_canvasWindowSize(0, 0),
	  m_canvasImageSize(0, 0) {}

void CoordinateSystem::setScreenSize(float width, float height) {
	m_screenSize = glm::vec2(width, height);
}

void CoordinateSystem::setAppWindow(int x, int y, float width, float height) {
	m_appPos = glm::vec2(x, y);
	m_appSize = glm::vec2(width, height);
}

void CoordinateSystem::setCanvasWindow(float x, float y, float width,
									   float height) {
	m_canvasWindowPos = glm::vec2(x, y);
	m_canvasWindowSize = glm::vec2(width, height);
}

void CoordinateSystem::setCanvasImage(float width, float height) {
	m_canvasImageSize = glm::vec2(width, height);
}

glm::vec2 CoordinateSystem::transform(glm::vec2 point, CoordinateSpace from,
									  CoordinateSpace to) {
	if (from == to)
		return point;

	// First convert to screen coordinates
	glm::vec2 screenPoint;
	switch (from) {
	case CoordinateSpace::Screen:
		screenPoint = point;
		break;
	case CoordinateSpace::App:
		screenPoint = appToScreen(point);
		break;
	case CoordinateSpace::Window:
		screenPoint = appToScreen(windowToApp(point));
		break;
	case CoordinateSpace::Canvas:
		screenPoint = appToScreen(windowToApp(canvasToWindow(point)));
		break;
	}

	// Then convert to target space
	switch (to) {
	case CoordinateSpace::Screen:
		return screenPoint;
	case CoordinateSpace::App:
		return screenToApp(screenPoint);
	case CoordinateSpace::Window:
		return appToWindow(screenToApp(screenPoint));
	case CoordinateSpace::Canvas:
		return windowToCanvas(appToWindow(screenToApp(screenPoint)));
	}

	return point;
}

CoordinateSystem::CoordinateInfo
CoordinateSystem::getCoordinateInfo(glm::vec2 mousePos, CoordinateSpace space) {
	CoordinateInfo info;
	info.mouse = mousePos;

	switch (space) {
	case CoordinateSpace::Screen:
		info.spaceName = "Screen";
		info.description = "Absolute screen coordinates";
		info.relative = mousePos;
		info.bounds = m_screenSize;
		break;

	case CoordinateSpace::App:
		info.spaceName = "App";
		info.description = "Application window coordinates";
		info.relative = screenToApp(mousePos);
		info.bounds = m_appSize;
		break;

	case CoordinateSpace::Window:
		info.spaceName = "Window";
		info.description = "Canvas window coordinates";
		info.relative = appToWindow(screenToApp(mousePos));
		info.bounds = m_canvasWindowSize;
		break;

	case CoordinateSpace::Canvas:
		info.spaceName = "Canvas";
		info.description = "Canvas image coordinates";
		info.relative = windowToCanvas(appToWindow(screenToApp(mousePos)));
		info.bounds = m_canvasImageSize;
		break;
	}

	return info;
}

glm::vec2 CoordinateSystem::getMousePosition() const {
	return glm::vec2(0, 0); // This would need to be updated from GLFW
}

glm::vec2 CoordinateSystem::getScreenSize() const { return m_screenSize; }

glm::vec2 CoordinateSystem::getAppWindowSize() const { return m_appSize; }

glm::vec2 CoordinateSystem::getCanvasWindowSize() const {
	return m_canvasWindowSize;
}

glm::vec2 CoordinateSystem::getCanvasImageSize() const {
	return m_canvasImageSize;
}

// Private helper functions
glm::vec2 CoordinateSystem::screenToApp(glm::vec2 point) {
	return point - m_appPos;
}

glm::vec2 CoordinateSystem::appToScreen(glm::vec2 point) {
	return point + m_appPos;
}

glm::vec2 CoordinateSystem::appToWindow(glm::vec2 point) {
	return point - m_canvasWindowPos;
}

glm::vec2 CoordinateSystem::windowToApp(glm::vec2 point) {
	return point + m_canvasWindowPos;
}

glm::vec2 CoordinateSystem::windowToCanvas(glm::vec2 point) {
	if (m_canvasWindowSize.x > 0 && m_canvasWindowSize.y > 0) {
		float scaleX = m_canvasImageSize.x / m_canvasWindowSize.x;
		float scaleY = m_canvasImageSize.y / m_canvasWindowSize.y;
		return glm::vec2(point.x * scaleX, point.y * scaleY);
	}
	return point;
}

glm::vec2 CoordinateSystem::canvasToWindow(glm::vec2 point) {
	if (m_canvasImageSize.x > 0 && m_canvasImageSize.y > 0) {
		float scaleX = m_canvasWindowSize.x / m_canvasImageSize.x;
		float scaleY = m_canvasWindowSize.y / m_canvasImageSize.y;
		return glm::vec2(point.x * scaleX, point.y * scaleY);
	}
	return point;
}

} // namespace blot
