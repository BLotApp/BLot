#include "DebugPanel.h"
#include "ecs/ECSManager.h"
#include "components/ShapeComponent.h"
#include "components/StyleComponent.h"
#include <imgui.h>
#include <iostream>

namespace blot {

DebugPanel::DebugPanel(const std::string& title, Flags flags)
    : Window(title, flags) {
}

void DebugPanel::render() {
    if (ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags)) {
        renderDebugInfo();
        renderClearShapesButton();
        renderEntityInfo();
        renderPerformanceInfo();
    }
    ImGui::End();
}

void DebugPanel::renderDebugInfo() {
    ImGui::Text("Debug Info:");
    ImGui::Text("  Delta Time: %.3f ms", m_deltaTime * 1000.0f);
    ImGui::Text("  Frame Rate: %.1f FPS", 1.0f / m_deltaTime);
}

void DebugPanel::renderClearShapesButton() {
    if (ImGui::Button("Clear All Shapes")) {
        printf("[DebugPanel] === Before Clear ===\n");
        printf("[DebugPanel] Total entities: %zu\n", m_ecs->getEntityCount());
        auto allEntities = m_ecs->getAllEntities();
        for (auto entity : allEntities) {
            bool hasTransform = m_ecs->hasComponent<Transform>(entity);
            bool hasShape = m_ecs->hasComponent<blot::components::Shape>(entity);
            bool hasStyle = m_ecs->hasComponent<blot::components::Style>(entity);
            printf("[DebugPanel] Entity %u: Transform=%d, Shape=%d, Style=%d\n", 
                   (unsigned int)entity, hasTransform, hasShape, hasStyle);
        }
        
        auto view = m_ecs->view<Transform, Shape, Style>();
        int count = 0;
        for (auto entity : view) {
            printf("[DebugPanel] Destroying entity %u\n", (unsigned int)entity);
            m_ecs->destroyEntity(entity);
            count++;
        }
        printf("[DebugPanel] Cleared %d shapes\n", count);
        
        printf("[DebugPanel] === After Clear ===\n");
        printf("[DebugPanel] Total entities: %zu\n", m_ecs->getEntityCount());
        allEntities = m_ecs->getAllEntities();
        for (auto entity : allEntities) {
            bool hasTransform = m_ecs->hasComponent<Transform>(entity);
            bool hasShape = m_ecs->hasComponent<blot::components::Shape>(entity);
            bool hasStyle = m_ecs->hasComponent<blot::components::Style>(entity);
            printf("[DebugPanel] Entity %u: Transform=%d, Shape=%d, Style=%d\n", 
                   (unsigned int)entity, hasTransform, hasShape, hasStyle);
        }
    }
}

void DebugPanel::renderEntityInfo() {
    if (m_ecs) {
        ImGui::Separator();
        ImGui::Text("Entity Info:");
        ImGui::Text("  Total Entities: %zu", m_ecs->getEntityCount());
        
        auto view = m_ecs->view<Transform, Shape, Style>();
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