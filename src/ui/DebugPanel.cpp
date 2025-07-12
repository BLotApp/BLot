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

// Window interface implementations
void DebugPanel::show() { m_state.isOpen = true; }
void DebugPanel::hide() { m_state.isOpen = false; }
void DebugPanel::close() { m_state.isOpen = false; }
void DebugPanel::toggle() { m_state.isOpen = !m_state.isOpen; }
bool DebugPanel::isOpen() const { return m_state.isOpen; }
bool DebugPanel::isVisible() const { return m_state.isOpen; }
bool DebugPanel::isFocused() const { return ImGui::IsWindowFocused(); }
bool DebugPanel::isHovered() const { return ImGui::IsWindowHovered(); }
bool DebugPanel::isDragging() const { return false; }
bool DebugPanel::isResizing() const { return false; }
void DebugPanel::setPosition(const ImVec2& pos) { /* TODO: Implement if needed */ }
void DebugPanel::setSize(const ImVec2& size) { /* TODO: Implement if needed */ }
void DebugPanel::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void DebugPanel::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void DebugPanel::setFlags(Window::Flags flags) { m_state.flags = static_cast<int>(flags); }
int DebugPanel::getFlags() const { return m_state.flags; }
std::string DebugPanel::getTitle() const { return m_title; }

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