#pragma once

#include <glm/glm.hpp>
#include <memory>
#include "ecs/MEcs.h"
#include "ecs/components/CDrawStyle.h"
#include "ecs/components/CShape.h"
#include "ecs/components/CTransform.h"
#include "rendering/IRenderer.h"

namespace blot {
namespace ecs {

// Main rendering function
void SShapeRendering(MEcs &ecs, std::shared_ptr<IRenderer> renderer);

// Individual shape rendering functions
void renderRectangle(const ecs::CTransform &transform, const ecs::CShape &shape,
					 const ecs::CDrawStyle &style,
					 std::shared_ptr<IRenderer> renderer);
void renderEllipse(const ecs::CTransform &transform, const ecs::CShape &shape,
				   const ecs::CDrawStyle &style,
				   std::shared_ptr<IRenderer> renderer);
void renderLine(const ecs::CTransform &transform, const ecs::CShape &shape,
				const ecs::CDrawStyle &style,
				std::shared_ptr<IRenderer> renderer);
void renderPolygon(const ecs::CTransform &transform, const ecs::CShape &shape,
				   const ecs::CDrawStyle &style,
				   std::shared_ptr<IRenderer> renderer);
void renderStar(const ecs::CTransform &transform, const ecs::CShape &shape,
				const ecs::CDrawStyle &style,
				std::shared_ptr<IRenderer> renderer);

// UI rendering for selection and preview
void renderSelectionOverlay(MEcs &ecs, const glm::vec2 &canvasPos,
							const glm::vec2 &canvasSize,
							std::shared_ptr<IRenderer> renderer);
void renderDrawingPreview(MEcs &ecs, const glm::vec2 &canvasPos,
						  const glm::vec2 &canvasSize,
						  std::shared_ptr<IRenderer> renderer);

// Helper functions
void setFillStyle(const ecs::CDrawStyle &style,
				  std::shared_ptr<IRenderer> renderer);
void setStrokeStyle(const ecs::CDrawStyle &style,
					std::shared_ptr<IRenderer> renderer);
void convertColor(float r, float g, float b, float a, uint32_t &color);

} // namespace ecs
} // namespace blot
