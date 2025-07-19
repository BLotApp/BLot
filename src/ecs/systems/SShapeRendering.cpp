#include "SShapeRendering.h"
#include <cmath>
#include "imgui.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include "ecs/components/CSelection.h"
#include "ecs/components/CTransform.h"
#include "rendering/IRenderer.h"

namespace blot {
namespace ecs {

// TODO: This should be a class that inherits from ISystem?
void SShapeRendering(MEcs &ecs,
						  std::shared_ptr<IRenderer> renderer) {
	// If Blend2D-specific logic is needed, use dynamic_cast here
	// Blend2DRenderer* blend2d =
	// dynamic_cast<Blend2DRenderer*>(renderer.get());
	auto view =
		ecs.view<ecs::CTransform, ecs::CShape, ecs::CDrawStyle>();

	for (auto entity : view) {
		auto &transform = view.get<ecs::CTransform>(entity);
		auto &shape = view.get<ecs::CShape>(entity);
		auto &style = view.get<ecs::CDrawStyle>(entity);

		switch (shape.type) {
		case ecs::CShape::Type::Rectangle:
			renderRectangle(transform, shape, style, renderer);
			break;
		case ecs::CShape::Type::Ellipse:
			renderEllipse(transform, shape, style, renderer);
			break;
		case ecs::CShape::Type::Line:
			renderLine(transform, shape, style, renderer);
			break;
		case ecs::CShape::Type::Polygon:
			renderPolygon(transform, shape, style, renderer);
			break;
		case ecs::CShape::Type::Star:
			renderStar(transform, shape, style, renderer);
			break;
		}
	}
}

void renderRectangle(const ecs::CTransform &transform,
					 const ecs::CShape &shape,
					 const ecs::CDrawStyle &style,
					 std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	float x = transform.position.x + shape.x1;
	float y = transform.position.y + shape.y1;
	float width = shape.x2 - shape.x1;
	float height = shape.y2 - shape.y1;

	// Apply transform
	x *= transform.scale.x;
	y *= transform.scale.y;
	width *= transform.scale.x;
	height *= transform.scale.y;

	if (style.hasFill) {
		setFillStyle(style, renderer);
		renderer->drawRect(x, y, width, height);
	}

	if (style.hasStroke) {
		setStrokeStyle(style, renderer);
		renderer->drawRect(x, y, width, height);
	}
}

void renderEllipse(const ecs::CTransform &transform,
				   const ecs::CShape &shape,
				   const ecs::CDrawStyle &style,
				   std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	float x = transform.position.x + shape.x1;
	float y = transform.position.y + shape.y1;
	float width = shape.x2 - shape.x1;
	float height = shape.y2 - shape.y1;

	// Apply transform
	x *= transform.scale.x;
	y *= transform.scale.y;
	width *= transform.scale.x;
	height *= transform.scale.y;

	float centerX = x + width * 0.5f;
	float centerY = y + height * 0.5f;
	float radiusX = width * 0.5f;
	float radiusY = height * 0.5f;

	if (style.hasFill) {
		setFillStyle(style, renderer);
		renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
	}

	if (style.hasStroke) {
		setStrokeStyle(style, renderer);
		renderer->drawEllipse(centerX, centerY, radiusX, radiusY);
	}
}

void renderLine(const ecs::CTransform &transform,
				const ecs::CShape &shape, const ecs::CDrawStyle &style,
				std::shared_ptr<IRenderer> renderer) {
	if (!renderer || !style.hasStroke)
		return;

	float x1 = transform.position.x + shape.x1;
	float y1 = transform.position.y + shape.y1;
	float x2 = transform.position.x + shape.x2;
	float y2 = transform.position.y + shape.y2;

	// Apply transform
	x1 *= transform.scale.x;
	y1 *= transform.scale.y;
	x2 *= transform.scale.x;
	y2 *= transform.scale.y;

	setStrokeStyle(style, renderer);
	renderer->drawLine(x1, y1, x2, y2);
}

void renderPolygon(const ecs::CTransform &transform,
				   const ecs::CShape &shape,
				   const ecs::CDrawStyle &style,
				   std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	float centerX = transform.position.x + shape.x1;
	float centerY = transform.position.y + shape.y1;
	float radius = shape.x2 - shape.x1;

	// Apply transform
	centerX *= transform.scale.x;
	centerY *= transform.scale.y;
	radius *= transform.scale.x;

	std::vector<float> points;
	int sides = shape.sides;
	for (int i = 0; i < sides; ++i) {
		float angle = 2.0f * M_PI * i / sides;
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		points.push_back(x);
		points.push_back(y);
	}

	std::vector<glm::vec2> glmPoints;
	for (size_t i = 0; i < points.size(); i += 2) {
		glmPoints.push_back(glm::vec2(points[i], points[i + 1]));
	}
	if (style.hasFill) {
		setFillStyle(style, renderer);
		renderer->drawPolygon(glmPoints);
	}

	if (style.hasStroke) {
		setStrokeStyle(style, renderer);
		renderer->drawPolygon(glmPoints);
	}
}

void renderStar(const ecs::CTransform &transform,
				const ecs::CShape &shape, const ecs::CDrawStyle &style,
				std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	float centerX = transform.position.x + shape.x1;
	float centerY = transform.position.y + shape.y1;
	float outerRadius = shape.x2 - shape.x1;
	float innerRadius = outerRadius * shape.innerRadius;

	// Apply transform
	centerX *= transform.scale.x;
	centerY *= transform.scale.y;
	outerRadius *= transform.scale.x;
	innerRadius *= transform.scale.x;

	std::vector<float> points;
	int points_count = shape.sides * 2;
	for (int i = 0; i < points_count; ++i) {
		float angle = 2.0f * M_PI * i / points_count;
		float radius = (i % 2 == 0) ? outerRadius : innerRadius;
		float x = centerX + radius * cos(angle);
		float y = centerY + radius * sin(angle);
		points.push_back(x);
		points.push_back(y);
	}

	std::vector<glm::vec2> glmPoints;
	for (size_t i = 0; i < points.size(); i += 2) {
		glmPoints.push_back(glm::vec2(points[i], points[i + 1]));
	}
	if (style.hasFill) {
		setFillStyle(style, renderer);
		renderer->drawPolygon(glmPoints);
	}

	if (style.hasStroke) {
		setStrokeStyle(style, renderer);
		renderer->drawPolygon(glmPoints);
	}
}

void renderSelectionOverlay(MEcs &ecs, const ImVec2 &canvasPos,
							const ImVec2 &canvasSize,
							std::shared_ptr<IRenderer> renderer) {
	auto view = ecs.view<ecs::CTransform, ecs::CShape,
						 ecs::CSelection>();

	for (auto entity : view) {
		auto &transform = view.get<ecs::CTransform>(entity);
		auto &shape = view.get<ecs::CShape>(entity);
		auto &selection = view.get<ecs::CSelection>(entity);

		// Draw selection rectangle
		float x = transform.position.x + shape.x1;
		float y = transform.position.y + shape.y1;
		float width = shape.x2 - shape.x1;
		float height = shape.y2 - shape.y1;

		// Apply transform
		x *= transform.scale.x;
		y *= transform.scale.y;
		width *= transform.scale.x;
		height *= transform.scale.y;

		// Convert to screen coordinates
		float screenX = canvasPos.x + x;
		float screenY = canvasPos.y + y;

		// Draw selection rectangle
		if (renderer) {
			renderer->setStrokeColor(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // Red
			renderer->setStrokeWidth(2.0f);
			renderer->drawRect(screenX - 2, screenY - 2, width + 4, height + 4);
		}
	}
}

void renderDrawingPreview(MEcs &ecs, const ImVec2 &canvasPos,
						  const ImVec2 &canvasSize,
						  std::shared_ptr<IRenderer> renderer) {
	auto view =
		ecs.view<ecs::CTransform, ecs::CShape, ecs::CDrawStyle>();

	for (auto entity : view) {
		auto &transform = view.get<ecs::CTransform>(entity);
		auto &shape = view.get<ecs::CShape>(entity);
		auto &style = view.get<ecs::CDrawStyle>(entity);

		// Convert to screen coordinates
		float x = canvasPos.x + transform.position.x + shape.x1;
		float y = canvasPos.y + transform.position.y + shape.y1;

		// Apply transform
		x *= transform.scale.x;
		y *= transform.scale.y;

		// Draw preview (simplified)
		if (renderer && style.hasStroke) {
			setStrokeStyle(style, renderer);
			switch (shape.type) {
			case ecs::CShape::Type::Rectangle:
				renderer->drawRect(x, y,
								   (shape.x2 - shape.x1) * transform.scale.x,
								   (shape.y2 - shape.y1) * transform.scale.y);
				break;
			case ecs::CShape::Type::Ellipse:
				renderer->drawEllipse(
					x + (shape.x2 - shape.x1) * 0.5f * transform.scale.x,
					y + (shape.y2 - shape.y1) * 0.5f * transform.scale.y,
					(shape.x2 - shape.x1) * 0.5f * transform.scale.x,
					(shape.y2 - shape.y1) * 0.5f * transform.scale.y);
				break;
			case ecs::CShape::Type::Line:
				renderer->drawLine(
					x, y, x + (shape.x2 - shape.x1) * transform.scale.x,
					y + (shape.y2 - shape.y1) * transform.scale.y);
				break;
			default:
				break;
			}
		}
	}
}

void setFillStyle(const ecs::CDrawStyle &style,
				  std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	glm::vec4 fillColor(style.fillR, style.fillG, style.fillB, style.fillA);
	renderer->setFillColor(fillColor);
}

void setStrokeStyle(const ecs::CDrawStyle &style,
					std::shared_ptr<IRenderer> renderer) {
	if (!renderer)
		return;

	glm::vec4 strokeColor(style.strokeR, style.strokeG, style.strokeB,
						  style.strokeA);
	renderer->setStrokeColor(strokeColor);
	renderer->setStrokeWidth(style.strokeWidth);
}

void convertColor(float r, float g, float b, float a, uint32_t &color) {
	uint8_t red = static_cast<uint8_t>(r * 255.0f);
	uint8_t green = static_cast<uint8_t>(g * 255.0f);
	uint8_t blue = static_cast<uint8_t>(b * 255.0f);
	uint8_t alpha = static_cast<uint8_t>(a * 255.0f);

	color = (alpha << 24) | (blue << 16) | (green << 8) | red;
}

} // namespace ecs
} // namespace blot
