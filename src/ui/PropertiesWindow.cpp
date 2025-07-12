#include "PropertiesWindow.h"
#include "ecs/ECSManager.h"
#include "components/TransformComponent.h"
#include "components/ShapeComponent.h"
#include "components/StyleComponent.h"
#include <imgui.h>

namespace blot {

PropertiesWindow::PropertiesWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
}

// Window interface implementations
void PropertiesWindow::show() { m_state.isOpen = true; }
void PropertiesWindow::hide() { m_state.isOpen = false; }
void PropertiesWindow::close() { m_state.isOpen = false; }
void PropertiesWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool PropertiesWindow::isOpen() const { return m_state.isOpen; }
bool PropertiesWindow::isVisible() const { return m_state.isOpen; }
bool PropertiesWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool PropertiesWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool PropertiesWindow::isDragging() const { return false; }
bool PropertiesWindow::isResizing() const { return false; }
void PropertiesWindow::setPosition(const ImVec2& pos) { /* TODO: Implement if needed */ }
void PropertiesWindow::setSize(const ImVec2& size) { /* TODO: Implement if needed */ }
void PropertiesWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void PropertiesWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void PropertiesWindow::setFlags(Window::Flags flags) { m_state.flags = static_cast<int>(flags); }
int PropertiesWindow::getFlags() const { return m_state.flags; }
std::string PropertiesWindow::getTitle() const { return m_title; }

void PropertiesWindow::setECSManager(std::shared_ptr<ECSManager> ecs) {
    m_ecs = ecs;
}

void PropertiesWindow::setSelectedEntity(entt::entity entity) {
    m_selectedEntity = entity;
    if (m_onEntitySelected) {
        m_onEntitySelected(entity);
    }
}

entt::entity PropertiesWindow::getSelectedEntity() const {
    return m_selectedEntity;
}

void PropertiesWindow::setOnEntitySelected(std::function<void(entt::entity)> callback) {
    m_onEntitySelected = callback;
}

void PropertiesWindow::setOnPropertyChanged(std::function<void(entt::entity, const std::string&, const std::string&)> callback) {
    m_onPropertyChanged = callback;
}

void PropertiesWindow::render() {
    begin();
    if (isOpen()) {
        renderEntityList();
        ImGui::Separator();
        
        if (m_selectedEntity != entt::null && m_ecs) {
            renderTransformProperties();
            renderShapeProperties();
            renderStyleProperties();
        }
        
        end();
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
        
        bool isSelected = (entity == m_selectedEntity);
        if (ImGui::Selectable(label, isSelected)) {
            setSelectedEntity(entity);
        }
    }
    
    ImGui::EndChild();
}

void PropertiesWindow::renderTransformProperties() {
    if (!m_ecs || m_selectedEntity == entt::null) return;
    
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<TransformComponent>(m_selectedEntity)) {
            auto& transform = m_ecs->getComponent<TransformComponent>(m_selectedEntity);
            
            renderVector2Editor("Position", transform.x, transform.y);
            renderVector2Editor("Scale", transform.scaleX, transform.scaleY);
            renderFloatEditor("Rotation", transform.rotation, 0.0f, 360.0f);
        }
    }
}

void PropertiesWindow::renderShapeProperties() {
    if (!m_ecs || m_selectedEntity == entt::null) return;
    
    if (ImGui::CollapsingHeader("Shape", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<ShapeComponent>(m_selectedEntity)) {
            auto& shape = m_ecs->getComponent<ShapeComponent>(m_selectedEntity);
            
            renderVector2Editor("Start", shape.x1, shape.y1);
            renderVector2Editor("End", shape.x2, shape.y2);
            
            const char* shapeTypes[] = { "Rectangle", "Ellipse", "Line", "Polygon", "Star" };
            int currentType = static_cast<int>(shape.type);
            if (ImGui::Combo("Type", &currentType, shapeTypes, IM_ARRAYSIZE(shapeTypes))) {
                shape.type = static_cast<ShapeComponent::Type>(currentType);
            }
            
            if (shape.type == ShapeComponent::Type::Polygon || shape.type == ShapeComponent::Type::Star) {
                renderIntEditor("Sides", shape.sides, 3, 20);
            }
        }
    }
}

void PropertiesWindow::renderStyleProperties() {
    if (!m_ecs || m_selectedEntity == entt::null) return;
    
    if (ImGui::CollapsingHeader("Style", ImGuiTreeNodeFlags_DefaultOpen)) {
        if (m_ecs->hasComponent<StyleComponent>(m_selectedEntity)) {
            auto& style = m_ecs->getComponent<StyleComponent>(m_selectedEntity);
            
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
                style.strokeCap = static_cast<StyleComponent::StrokeCap>(currentCap);
            }
            
            const char* strokeJoins[] = { "Miter", "Bevel", "Round" };
            int currentJoin = static_cast<int>(style.strokeJoin);
            if (ImGui::Combo("Stroke Join", &currentJoin, strokeJoins, IM_ARRAYSIZE(strokeJoins))) {
                style.strokeJoin = static_cast<StyleComponent::StrokeJoin>(currentJoin);
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