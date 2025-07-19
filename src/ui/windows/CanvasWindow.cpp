#ifndef NOMINMAX
#define NOMINMAX
#endif
#include "CanvasWindow.h"
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "ImGuiWindow.h"
#include "core/canvas/Canvas.h"
#include "ecs/MEcs.h"
#include "ecs/components/CDrawStyle.h"
#include "ecs/components/CShape.h"
#include "ecs/components/CTransform.h"
#include "rendering/Graphics.h"
#include "rendering/MRendering.h"

namespace blot {

CanvasWindow::CanvasWindow(const std::string &title, Flags flags)
	: Window(title, flags) {
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

void CanvasWindow::setToolStartPos(const ImVec2 &pos) { m_toolStartPos = pos; }

void CanvasWindow::setToolActive(bool active) { m_toolActive = active; }

void CanvasWindow::setFillColor(const ImVec4 &color) { m_fillColor = color; }

void CanvasWindow::setStrokeColor(const ImVec4 &color) {
	m_strokeColor = color;
}

void CanvasWindow::setStrokeWidth(float width) { m_strokeWidth = width; }

void CanvasWindow::handleMouseInput() {
	if (!ImGui::IsWindowHovered()) {
		return;
	}

	// Check if we're dragging the window (mouse over title bar)
	bool isDraggingWindow =
		ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
		ImGui::GetIO().MouseClickedPos[0].y < ImGui::GetCursorPosY();

	// Always allow window dragging regardless of tool state
	if (isDraggingWindow) {
		// Window is being dragged, don't handle tool input
		return;
	}

	ImVec2 mousePos = ImGui::GetMousePos();

	// Check if mouse is inside canvas bounds
	m_mouseInsideCanvas =
		(mousePos.x >= m_canvasPos.x && mousePos.x <= m_canvasEnd.x &&
		 mousePos.y >= m_canvasPos.y && mousePos.y <= m_canvasEnd.y);

	if (m_mouseInsideCanvas) {
		m_canvasMousePos = convertToCanvasCoordinates(mousePos);
		handleShapeCreation();
	}
}

ImVec2 CanvasWindow::getCanvasMousePos() const { return m_canvasMousePos; }

bool CanvasWindow::isMouseInsideCanvas() const { return m_mouseInsideCanvas; }

void CanvasWindow::renderContents() {
	// Get the canvas area
	m_canvasPos = ImGui::GetCursorScreenPos();
	m_canvasSize = ImGui::GetContentRegionAvail();
	m_canvasEnd =
		ImVec2(m_canvasPos.x + m_canvasSize.x, m_canvasPos.y + m_canvasSize.y);

	// Draw the canvas texture
	drawCanvasTexture();

	// Handle mouse input
	handleMouseInput();
}

void CanvasWindow::drawCanvasTexture() {
	if (!m_renderingManager || m_activeCanvasId == entt::null) {
		return;
	}

	auto canvasPtr = m_renderingManager->getCanvas(m_activeCanvasId);
	if (canvasPtr && *canvasPtr) {
		unsigned int texId = (*canvasPtr)->getColorTexture();
		spdlog::debug("[ImGui] Displaying texture: ID={}, size={}x{}", texId,
					  m_canvasSize.x, m_canvasSize.y);

		// Draw the image
		ImGui::Image((void *)(intptr_t)texId, m_canvasSize);

		// Update canvas position to match the actual image position
		m_canvasPos = ImGui::GetItemRectMin();
		m_canvasEnd = ImGui::GetItemRectMax();
		m_canvasSize = ImVec2(m_canvasEnd.x - m_canvasPos.x,
							  m_canvasEnd.y - m_canvasPos.y);

		spdlog::debug(
			"[ImGui] Image bounds: pos=({},{}), size=({},{}), end=({},{}))",
			m_canvasPos.x, m_canvasPos.y, m_canvasSize.x, m_canvasSize.y,
			m_canvasEnd.x, m_canvasEnd.y);
	} else {
		spdlog::error(
			"[ImGui] ERROR: No canvas resource found for active canvas");
	}
}

void CanvasWindow::handleShapeCreation() {
	if (m_currentTool == 0) { // Rectangle tool
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			spdlog::debug("[Mouse] Left mouse clicked! canvasMousePos=({}, {})",
						  m_canvasMousePos.x, m_canvasMousePos.y);
			m_toolStartPos = m_canvasMousePos;
			m_toolActive = true;
		}

		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_toolActive) {
			spdlog::debug("[Mouse] Left mouse released, creating shape...");
			spdlog::debug("[Mouse] DEBUG: Tool was active, creating shape with "
						  "start=({}, {}), end=({}, {})",
						  m_toolStartPos.x, m_toolStartPos.y,
						  m_canvasMousePos.x, m_canvasMousePos.y);
			createShape(m_toolStartPos, m_canvasMousePos);
			m_toolActive = false;
		}
	}
}

void CanvasWindow::createShape(const ImVec2 &start, const ImVec2 &end) {
	if (!m_ecs) {
		spdlog::error("[CanvasWindow] ERROR: No ECS manager available");
		return;
	}

	spdlog::debug(
		"[CanvasWindow] Input coordinates: start=({}, {}), end=({}, {})",
		start.x, start.y, end.x, end.y);

	ImVec2 blend2DStart = convertToBlend2DCoordinates(start);
	ImVec2 blend2DEnd = convertToBlend2DCoordinates(end);

	spdlog::debug(
		"[CanvasWindow] Converted coordinates: start=({}, {}), end=({}, {})",
		blend2DStart.x, blend2DStart.y, blend2DEnd.x, blend2DEnd.y);

	float x1 = std::min(blend2DStart.x, blend2DEnd.x);
	float y1 = std::min(blend2DStart.y, blend2DEnd.y);
	float x2 = std::max(blend2DStart.x, blend2DEnd.x);
	float y2 = std::max(blend2DStart.y, blend2DEnd.y);

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

ImVec2 CanvasWindow::convertToCanvasCoordinates(const ImVec2 &screenPos) const {
	return ImVec2(screenPos.x - m_canvasPos.x, screenPos.y - m_canvasPos.y);
}

ImVec2
CanvasWindow::convertToBlend2DCoordinates(const ImVec2 &canvasPos) const {
	spdlog::debug("[CanvasWindow] convertToBlend2DCoordinates: input=({}, {})",
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

	ImVec2 result = ImVec2(canvasPos.x * scaleX, canvasPos.y * scaleY);
	spdlog::debug("[CanvasWindow] Scale: ({}x{}, result: ({}x{})", scaleX,
				  scaleY, result.x, result.y);

	return result;
}

} // namespace blot
