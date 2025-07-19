#include "DebugPanel.h"
#include <imgui.h>
#include <spdlog/spdlog.h>
#include "ecs/ECSManager.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TransformComponent.h"

namespace blot {

DebugPanel::DebugPanel(const std::string &title, Flags flags)
	: Window(title, flags) {}

void DebugPanel::renderContents() {
	renderDebugInfo();
	renderClearShapesButton();
	renderEntityInfo();
	renderPerformanceInfo();
}

void DebugPanel::renderDebugInfo() {
	ImGui::Text("Debug Info:");
	ImGui::Text("  Delta Time: %.3f ms", m_deltaTime * 1000.0f);
	ImGui::Text("  Frame Rate: %.1f FPS", 1.0f / m_deltaTime);
}

void DebugPanel::renderClearShapesButton() {
	if (ImGui::Button("Clear All Shapes")) {
		spdlog::debug("[DebugPanel] === Before Clear ===");
		spdlog::debug("[DebugPanel] Total entities: {}",
					  m_ecs->getEntityCount());
		auto allEntities = m_ecs->getAllEntities();
		for (auto entity : allEntities) {
			bool hasTransform =
				m_ecs->hasComponent<components::Transform>(entity);
			bool hasShape = m_ecs->hasComponent<components::Shape>(entity);
			bool hasStyle = m_ecs->hasComponent<components::Style>(entity);
			spdlog::debug(
				"[DebugPanel] Entity {}: Transform={}, Shape={}, Style={}",
				(unsigned int)entity, hasTransform, hasShape, hasStyle);
		}

		auto view = m_ecs->view<components::Transform, components::Shape,
								components::Style>();
		int count = 0;
		for (auto entity : view) {
			spdlog::debug("[DebugPanel] Destroying entity {}",
						  (unsigned int)entity);
			m_ecs->destroyEntity(entity);
			count++;
		}
		spdlog::debug("[DebugPanel] Cleared {} shapes", count);

		spdlog::debug("[DebugPanel] === After Clear ===");
		spdlog::debug("[DebugPanel] Total entities: {}",
					  m_ecs->getEntityCount());
		allEntities = m_ecs->getAllEntities();
		for (auto entity : allEntities) {
			bool hasTransform =
				m_ecs->hasComponent<components::Transform>(entity);
			bool hasShape = m_ecs->hasComponent<components::Shape>(entity);
			bool hasStyle = m_ecs->hasComponent<components::Style>(entity);
			spdlog::debug(
				"[DebugPanel] Entity {}: Transform={}, Shape={}, Style={}",
				(unsigned int)entity, hasTransform, hasShape, hasStyle);
		}
	}
}

void DebugPanel::renderEntityInfo() {
	if (m_ecs) {
		ImGui::Separator();
		ImGui::Text("Entity Info:");
		ImGui::Text("  Total Entities: %zu", m_ecs->getEntityCount());

		auto view = m_ecs->view<components::Transform, components::Shape,
								components::Style>();
		size_t shapeCount = 0;
		for (auto entity : view) {
			shapeCount++;
		}
		ImGui::Text("  Shapes: %zu", shapeCount);
	}
}

void DebugPanel::renderPerformanceInfo() {
	ImGui::Separator();
	ImGui::Text("Performance:");
	ImGui::Text("  Memory Usage: TODO");
	ImGui::Text("  GPU Memory: TODO");
}

} // namespace blot
