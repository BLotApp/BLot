#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "CanvasWindow.h"
#include <spdlog/spdlog.h>
#include "core/canvas/Canvas.h"
#include "ecs/MEcs.h"
#include "ecs/components/CDrawStyle.h"
#include "ecs/components/CShape.h"
#include "ecs/components/CTransform.h"
#include "rendering/Graphics.h"
#include "rendering/MRendering.h"

namespace blot {

CanvasWindow::CanvasWindow(const std::string &title, WindowFlags flags)
	: m_title(title), m_flags(flags) {
	// Note: Window size and position are handled by ImGui automatically
}

void CanvasWindow::setRenderingManager(MRendering *renderingManager) {
	m_renderingManager = renderingManager;
}

void CanvasWindow::setECSManager(blot::MEcs *ecs) { m_ecs = ecs; }

void CanvasWindow::setActiveCanvasId(entt::entity canvasId) {
	m_activeCanvasId = canvasId;
}

void CanvasWindow::setCurrentTool(int toolType) { m_currentTool = toolType; }

void CanvasWindow::setToolStartPos(const glm::vec2 &pos) {
	m_toolStartPos = pos;
}

void CanvasWindow::setToolActive(bool active) { m_toolActive = active; }

void CanvasWindow::setFillColor(const glm::vec4 &color) { m_fillColor = color; }

void CanvasWindow::setStrokeColor(const glm::vec4 &color) {
	m_strokeColor = color;
}

void CanvasWindow::setStrokeWidth(float width) { m_strokeWidth = width; }

void CanvasWindow::handleMouseInput() {
	// This is a placeholder implementation
	// The actual mouse handling should be implemented by the UI system
	// that uses this CanvasWindow
}

glm::vec2 CanvasWindow::getCanvasMousePos() const { return m_canvasMousePos; }

bool CanvasWindow::isMouseInsideCanvas() const { return m_mouseInsideCanvas; }

void CanvasWindow::renderContents() {
	// This is a placeholder implementation
	// The actual rendering should be implemented by the UI system
	// that uses this CanvasWindow
}

void CanvasWindow::drawCanvasTexture() {
	// This is a placeholder implementation
	// The actual texture drawing should be implemented by the UI system
	// that uses this CanvasWindow
}

void CanvasWindow::handleShapeCreation() {
	// This is a placeholder implementation
	// The actual shape creation should be implemented by the UI system
	// that uses this CanvasWindow
}

void CanvasWindow::createShape(const glm::vec2 &start, const glm::vec2 &end) {
	if (!m_ecs) {
		spdlog::error("[CanvasWindow] ERROR: No ECS manager available");
		return;
	}

	spdlog::debug(
		"[CanvasWindow] Input coordinates: start=({}, {}), end=({}, {})",
		start.x, start.y, end.x, end.y);

	glm::vec2 rendererStart = convertToRendererCoordinates(start);
	glm::vec2 rendererEnd = convertToRendererCoordinates(end);

	spdlog::debug(
		"[CanvasWindow] Converted coordinates: start=({}, {}), end=({}, {})",
		rendererStart.x, rendererStart.y, rendererEnd.x, rendererEnd.y);

	float x1 = std::min(rendererStart.x, rendererEnd.x);
	float y1 = std::min(rendererStart.y, rendererEnd.y);
	float x2 = std::max(rendererStart.x, rendererEnd.x);
	float y2 = std::max(rendererStart.y, rendererEnd.y);

	spdlog::debug(
		"[CanvasWindow] Final shape coordinates: ({} to {}, {} to {})", x1, y1,
		x2, y2);

	// Create ECS entity with shape components
	entt::entity shapeEntity = m_ecs->createEntity();

	// Add Transform component
	blot::ecs::CTransform transform;
	transform.position.x = 0.0f;
	transform.position.y = 0.0f;
	transform.scale.x = 1.0f;
	transform.scale.y = 1.0f;
	m_ecs->addComponent<blot::ecs::CTransform>(shapeEntity, transform);

	// Add Shape component
	blot::ecs::CShape shape;
	shape.type = blot::ecs::CShape::Type::Rectangle;
	shape.x1 = x1;
	shape.y1 = y1;
	shape.x2 = x2;
	shape.y2 = y2;
	m_ecs->addComponent<blot::ecs::CShape>(shapeEntity, shape);

	// Add CDrawStyle component
	blot::ecs::CDrawStyle style;
	style.setFillColor(m_fillColor.x, m_fillColor.y, m_fillColor.z,
					   m_fillColor.w);
	style.setStrokeColor(m_strokeColor.x, m_strokeColor.y, m_strokeColor.z,
						 m_strokeColor.w);
	style.setStrokeWidth(m_strokeWidth);
	style.hasFill = true;
	style.hasStroke = true;
	m_ecs->addComponent<blot::ecs::CDrawStyle>(shapeEntity, style);

	spdlog::debug("[CanvasWindow] Created shape entity: {}",
				  static_cast<unsigned int>(shapeEntity));
}

glm::vec2
CanvasWindow::convertToCanvasCoordinates(const glm::vec2 &screenPos) const {
	return glm::vec2(screenPos.x - m_canvasPos.x, screenPos.y - m_canvasPos.y);
}

glm::vec2
CanvasWindow::convertToRendererCoordinates(const glm::vec2 &canvasPos) const {
	spdlog::debug("[CanvasWindow] convertToRendererCoordinates: input=({}, {})",
				  canvasPos.x, canvasPos.y);

	// Get actual canvas dimensions from the canvas resource
	if (!m_renderingManager || m_activeCanvasId == entt::null) {
		spdlog::error(
			"[CanvasWindow] ERROR: No rendering manager or active canvas");
		return canvasPos; // Return unchanged if no canvas available
	}

	auto canvasPtr = m_renderingManager->getCanvas(m_activeCanvasId);
	if (!canvasPtr || !*canvasPtr) {
		spdlog::error(
			"[CanvasWindow] ERROR: Active canvas not found in resources");
		return canvasPos; // Return unchanged if canvas not found
	}

	auto graphics = (*canvasPtr)->getGraphics();
	if (!graphics) {
		spdlog::error("[CanvasWindow] ERROR: No graphics object in canvas");
		return canvasPos; // Return unchanged if no graphics
	}

	auto renderer = graphics->getRenderer();
	if (!renderer) {
		spdlog::error("[CanvasWindow] ERROR: No renderer in graphics object");
		return canvasPos; // Return unchanged if no renderer
	}

	float canvasWidth = static_cast<float>(renderer->getWidth());
	float canvasHeight = static_cast<float>(renderer->getHeight());

	spdlog::debug("[CanvasWindow] Canvas dimensions: {}x{}, ImGui size: {}x{}",
				  canvasWidth, canvasHeight, m_canvasSize.x, m_canvasSize.y);

	if (m_canvasSize.x <= 0.0f || m_canvasSize.y <= 0.0f) {
		spdlog::error("[CanvasWindow] ERROR: Invalid canvas size: {}x{}",
					  m_canvasSize.x, m_canvasSize.y);
		return canvasPos; // Return unchanged if invalid size
	}

	float scaleX = canvasWidth / m_canvasSize.x;
	float scaleY = canvasHeight / m_canvasSize.y;

	glm::vec2 result = glm::vec2(canvasPos.x * scaleX, canvasPos.y * scaleY);
	spdlog::debug("[CanvasWindow] Scale: ({}x{}, result: ({}x{})", scaleX,
				  scaleY, result.x, result.y);

	return result;
}

// IWindow interface implementations
void CanvasWindow::show() { m_isVisible = true; }

void CanvasWindow::hide() { m_isVisible = false; }

void CanvasWindow::close() { m_isVisible = false; }

bool CanvasWindow::isVisible() const { return m_isVisible; }

void CanvasWindow::setTitle(const std::string &title) { m_title = title; }

std::string CanvasWindow::getTitle() const { return m_title; }

void CanvasWindow::setFlags(WindowFlags flags) { m_flags = flags; }

WindowFlags CanvasWindow::getFlags() const { return m_flags; }

void CanvasWindow::setPosition(int x, int y) {
	m_positionX = x;
	m_positionY = y;
}

void CanvasWindow::getPosition(int &x, int &y) const {
	x = m_positionX;
	y = m_positionY;
}

void CanvasWindow::setSize(int width, int height) {
	m_sizeWidth = width;
	m_sizeHeight = height;
}

void CanvasWindow::getSize(int &width, int &height) const {
	width = m_sizeWidth;
	height = m_sizeHeight;
}

} // namespace blot
