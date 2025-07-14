#include "PropertiesWindow.h"
#include "ecs/ECSManager.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include <imgui.h>

namespace blot {

PropertiesWindow::PropertiesWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
}

void PropertiesWindow::setECSManager(std::shared_ptr<ECSManager> ecs) {
    m_ecs = ecs;
}

void PropertiesWindow::setSelectedEntity(uint32_t entity) {
    m_selectedEntity = entity;
    if (m_onEntitySelected) {
        m_onEntitySelected(entity);
    }
}

uint32_t PropertiesWindow::getSelectedEntity() const {
    return m_selectedEntity;
}

void PropertiesWindow::setOnEntitySelected(std::function<void(uint32_t)> callback) {
    m_onEntitySelected = callback;
}

void PropertiesWindow::setOnPropertyChanged(std::function<void(uint32_t, const std::string&, const std::string&)> callback) {
    m_onPropertyChanged = callback;
}

void PropertiesWindow::renderContents() {
    renderEntityList();
    ImGui::Separator();
    if (m_selectedEntity != 0 && m_ecs) {
        renderTransformProperties();
        renderShapeProperties();
        renderStyleProperties();
    }
}

void PropertiesWindow::renderEntityList() {
    if (!m_ecs) return;
    
    ImGui::Text("Entities:");
    ImGui::BeginChild("EntityList", ImVec2(0, 100), true);
    
    auto view = m_ecs->view<TransformComponent>();
    for (auto entity : view) {
        char label[64];
        snprintf(label, sizeof(label), "Entity %u", (unsigned int)entity);
        
        bool isSelected = (entity == static_cast<entt::entity>(m_selectedEntity));
        if (ImGui::Selectable(label, isSelected)) {
            setSelectedEntity(static_cast<uint32_t>(entity));
        }
    }
    
    ImGui::EndChild();
}

void PropertiesWindow::renderTransformProperties() {
    if (!m_ecs || m_selectedEntity == 0) return;
    
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<TransformComponent>(static_cast<entt::entity>(m_selectedEntity))) {
            auto& transform = m_ecs->getComponent<TransformComponent>(static_cast<entt::entity>(m_selectedEntity));
            
            renderVector2Editor("Position", transform.position.x, transform.position.y);
            renderVector2Editor("Scale", transform.scale.x, transform.scale.y);
            renderFloatEditor("Rotation", transform.rotation, 0.0f, 360.0f);
        }
    }
}

void PropertiesWindow::renderShapeProperties() {
    if (!m_ecs || m_selectedEntity == 0) return;
    
    if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<blot::components::Shape>(static_cast<entt::entity>(m_selectedEntity))) {
            auto& shape = m_ecs->getComponent<blot::components::Shape>(static_cast<entt::entity>(m_selectedEntity));
            
            renderVector2Editor("Start", shape.x1, shape.y1);
            renderVector2Editor("End", shape.x2, shape.y2);
            
            const char* shapeTypes[] = { "Rectangle", "Ellipse", "Line", "Polygon", "Star" };
            int currentType = static_cast<int>(shape.type);
            if (ImGui::Combo("Type", &currentType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
                shape.type = static_cast<blot::components::Shape::Type>(currentType);
            }
            
            if (shape.type == blot::components::Shape::Type::Polygon || shape.type == blot::components::Shape::Type::Star) {
                renderIntEditor("Sides", shape.sides, 3, 20);
            }
        }
    }
}

void PropertiesWindow::renderStyleProperties() {
    if (!m_ecs || m_selectedEntity == 0) return;
    
    if (ImGui::CollapsingHeader("Style", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<blot::components::Style>(static_cast<entt::entity>(m_selectedEntity))) {
            auto& style = m_ecs->getComponent<blot::components::Style>(static_cast<entt::entity>(m_selectedEntity));
            
            ImVec4 fillColor(style.fillR, style.fillG, style.fillB, style.fillA);
            if (renderColorPicker("Fill Color", fillColor)) {
                style.fillR = fillColor.x;
                style.fillG = fillColor.y;
                style.fillB = fillColor.z;
                style.fillA = fillColor.w;
            }
            
            ImVec4 strokeColor(style.strokeR, style.strokeG, style.strokeB, style.strokeA);
            if (renderColorPicker("Stroke Color", strokeColor)) {
                style.strokeR = strokeColor.x;
                style.strokeG = strokeColor.y;
                style.strokeB = strokeColor.z;
                style.strokeA = strokeColor.w;
            }
            
            renderFloatEditor("Stroke Width", style.strokeWidth, 0.0f, 20.0f);
            
            const char* strokeCaps[] = { "Butt", "Square", "Round" };
            int currentCap = static_cast<int>(style.strokeCap);
            if (ImGui::Combo("Stroke Cap", &currentCap, strokeCaps, IM_ARRAYSIZE(strokeCaps))) {
                style.strokeCap = static_cast<blot::components::Style::StrokeCap>(currentCap);
            }
            
            const char* strokeJoins[] = { "Miter", "Bevel", "Round" };
            int currentJoin = static_cast<int>(style.strokeJoin);
            if (ImGui::Combo("Stroke Join", &currentJoin, strokeJoins, IM_ARRAYSIZE(strokeJoins))) {
                style.strokeJoin = static_cast<blot::components::Style::StrokeJoin>(currentJoin);
            }
        }
    }
}

bool PropertiesWindow::renderColorPicker(const char* label, ImVec4& color) {
    return ImGui::ColorEdit4(label, (float*)&color, ImGuiColorEditFlags_NoInputs);
}

void PropertiesWindow::renderVector2Editor(const char* label, float& x, float& y) {
    ImGui::Text("%s:", label);
    ImGui::SameLine();
    ImGui::PushItemWidth(60);
    ImGui::DragFloat(("##" + std::string(label) + "X").c_str(), &x, 0.1f);
    ImGui::SameLine();
    ImGui::DragFloat(("##" + std::string(label) + "Y").c_str(), &y, 0.1f);
    ImGui::PopItemWidth();
}

void PropertiesWindow::renderFloatEditor(const char* label, float& value, float min, float max) {
    ImGui::DragFloat(label, &value, 0.1f, min, max);
}

void PropertiesWindow::renderIntEditor(const char* label, int& value, int min, int max) {
    ImGui::DragInt(label, &value, 1, min, max);
}

} // namespace blot 