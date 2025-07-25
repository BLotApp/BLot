#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>
#include <memory>
#include "core/IWindow.h"
#include "core/canvas/Canvas.h"
#include "ecs/MEcs.h"
#include "rendering/MRendering.h"

namespace blot {

class MEcs;

class CanvasWindow : public IWindow {
  public:
	CanvasWindow(const std::string &title = "Canvas###MainCanvas",
				 WindowFlags flags = WindowFlags::None);
	virtual ~CanvasWindow() = default;

	// IWindow interface implementation
	void show() override;
	void hide() override;
	void close() override;
	bool isVisible() const override;
	void setTitle(const std::string &title) override;
	std::string getTitle() const override;
	void setFlags(WindowFlags flags) override;
	WindowFlags getFlags() const override;
	void setPosition(int x, int y) override;
	void getPosition(int &x, int &y) const override;
	void setSize(int width, int height) override;
	void getSize(int &width, int &height) const override;
	void renderContents() override;

	// Canvas-specific methods
	void setRenderingManager(MRendering *renderingManager);
	void setECSManager(blot::MEcs *ecs);
	void setActiveCanvasId(entt::entity canvasId);
	void setCurrentTool(int toolType);
	void setToolStartPos(const glm::vec2 &pos);
	void setToolActive(bool active);
	void setFillColor(const glm::vec4 &color);
	void setStrokeColor(const glm::vec4 &color);
	void setStrokeWidth(float width);

	// Mouse interaction
	void handleMouseInput();
	glm::vec2 getCanvasMousePos() const;
	bool isMouseInsideCanvas() const;

  private:
	// IWindow state
	std::string m_title;
	bool m_isVisible = true;
	WindowFlags m_flags = WindowFlags::None;
	int m_positionX = 0, m_positionY = 0;
	int m_sizeWidth = 400, m_sizeHeight = 300;

	// Canvas state
	MRendering *m_renderingManager = nullptr;
	blot::MEcs *m_ecs = nullptr;
	entt::entity m_activeCanvasId = entt::null;

	// Tool state
	int m_currentTool = 0;
	glm::vec2 m_toolStartPos = glm::vec2(0, 0);
	bool m_toolActive = false;

	// CDrawStyle state
	glm::vec4 m_fillColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 m_strokeColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	float m_strokeWidth = 2.0f;

	// Mouse state
	glm::vec2 m_canvasPos = glm::vec2(0, 0);
	glm::vec2 m_canvasSize = glm::vec2(0, 0);
	glm::vec2 m_canvasEnd = glm::vec2(0, 0);
	glm::vec2 m_canvasMousePos = glm::vec2(0, 0);
	bool m_mouseInsideCanvas = false;

	// Helper methods
	void drawCanvasTexture();
	void handleShapeCreation();
	void createShape(const glm::vec2 &start, const glm::vec2 &end);
	glm::vec2 convertToCanvasCoordinates(const glm::vec2 &screenPos) const;
	glm::vec2 convertToRendererCoordinates(const glm::vec2 &canvasPos) const;
};

} // namespace blot
