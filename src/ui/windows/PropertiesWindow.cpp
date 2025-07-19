#include "ui/windows/PropertiesWindow.h"
#include "ecs/ECSManager.h"
#include "ecs/components/CanvasComponent.h"
#include "ecs/components/DraggableComponent.h"
#include "ecs/components/NodeComponent.h"
#include "ecs/components/SelectableComponent.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TextureComponent.h"
#include "ecs/components/TransformComponent.h"
#include <array>
#include <imgui.h>

namespace blot {
using namespace components;

PropertiesWindow::PropertiesWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

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

void PropertiesWindow::setOnEntitySelected(
	std::function<void(uint32_t)> callback) {
	m_onEntitySelected = callback;
}

void PropertiesWindow::setOnPropertyChanged(
	std::function<void(uint32_t, const std::string &, const std::string &)>
		callback) {
	m_onPropertyChanged = callback;
}

void PropertiesWindow::renderContents() {
	renderEntityList();
	ImGui::Separator();
	if (m_selectedEntity != 0 && m_ecs) {
		renderAllComponentProperties();
	}
}

void PropertiesWindow::renderEntityList() {
	if (!m_ecs)
		return;
	ImGui::Text("Entities:");
	ImGui::BeginChild("EntityList", ImVec2(0, 100), true);
	auto view = m_ecs->view<components::Transform>();
	for (auto entity : view) {
		char label[64];
		snprintf(label, sizeof(label), "Entity %u", (unsigned int)entity);
		bool isSelected =
			(entity == static_cast<entt::entity>(m_selectedEntity));
		if (ImGui::Selectable(label, isSelected)) {
			setSelectedEntity(static_cast<uint32_t>(entity));
		}
	}
	ImGui::EndChild();
}

// --- New generic property rendering ---

template <typename Component>
void RenderComponentProperties(entt::entity entity, ECSManager *ecs,
							   const char *headerName) {
	if (ecs->hasComponent<Component>(entity)) {
		auto &comp = ecs->getComponent<Component>(entity);
		auto props = TryGetProperties(comp);
		if (!props.empty()) {
			if (ImGui::CollapsingHeader(headerName,
										ImGuiTreeNodeFlags_DefaultOpen)) {
				for (auto &prop : props) {
					switch (prop.type) {
					case EPT_BOOL:
						ImGui::Checkbox(prop.name.c_str(),
										static_cast<bool *>(prop.data));
						break;
					case EPT_INT:
						ImGui::DragInt(prop.name.c_str(),
									   static_cast<int *>(prop.data));
						break;
					case EPT_UINT:
						ImGui::DragInt(prop.name.c_str(),
									   static_cast<int *>(
										   prop.data)); // ImGui has no DragUInt
						break;
					case EPT_FLOAT:
						ImGui::DragFloat(prop.name.c_str(),
										 static_cast<float *>(prop.data), 0.1f);
						break;
					case EPT_DOUBLE: {
						// ImGui does not have DragDouble, use DragScalar
						auto doublePtr = static_cast<double *>(prop.data);
						if (doublePtr) {
							ImGui::DragScalar(prop.name.c_str(),
											  ImGuiDataType_Double, doublePtr,
											  0.1);
						}
						break;
					}
					case EPT_STRING: {
						auto strPtr = static_cast<std::string *>(prop.data);
						if (strPtr) {
							ImGuiInputTextStdString(prop.name.c_str(), *strPtr);
						}
						break;
					}
					case EPT_IMVEC4: {
						// Assume data points to 4 floats (e.g., glm::vec4 or
						// ImVec4)
						ImGui::ColorEdit4(prop.name.c_str(),
										  reinterpret_cast<float *>(prop.data));
						break;
					}
					default:
						ImGui::Text("%s (unsupported type)", prop.name.c_str());
						break;
					}
				}
			}
		}
	}
}

void PropertiesWindow::renderAllComponentProperties() {
	entt::entity entity = static_cast<entt::entity>(m_selectedEntity);
	ECSManager *ecs = m_ecs.get();
	RenderComponentProperties<components::Transform>(entity, ecs, "Transform");
	RenderComponentProperties<components::Shape>(entity, ecs, "Shape");
	RenderComponentProperties<components::Style>(entity, ecs, "Style");
	RenderComponentProperties<components::DraggableComponent>(entity, ecs,
															  "Draggable");
	RenderComponentProperties<components::SelectableComponent>(entity, ecs,
															   "Selectable");
	RenderComponentProperties<components::TextureComponent>(entity, ecs,
															"Texture");
	RenderComponentProperties<components::CanvasComponent>(entity, ecs,
														   "Canvas");
	// Add more as needed
}

// --- End new generic property rendering ---

bool PropertiesWindow::renderColorPicker(const char *label, ImVec4 &color) {
	return ImGui::ColorEdit4(label, (float *)&color,
							 ImGuiColorEditFlags_NoInputs);
}

void PropertiesWindow::renderVector2Editor(const char *label, float &x,
										   float &y) {
	ImGui::Text("%s:", label);
	ImGui::SameLine();
	ImGui::PushItemWidth(60);
	ImGui::DragFloat(("##" + std::string(label) + "X").c_str(), &x, 0.1f);
	ImGui::SameLine();
	ImGui::DragFloat(("##" + std::string(label) + "Y").c_str(), &y, 0.1f);
	ImGui::PopItemWidth();
}

void PropertiesWindow::renderFloatEditor(const char *label, float &value,
										 float min, float max) {
	ImGui::DragFloat(label, &value, 0.1f, min, max);
}

void PropertiesWindow::renderIntEditor(const char *label, int &value, int min,
									   int max) {
	ImGui::DragInt(label, &value, 1, min, max);
}

// Helper for ImGui string editing
inline bool ImGuiInputTextStdString(const char *label, std::string &str,
									size_t maxLen = 256) {
	char buf[256];
	strncpy(buf, str.c_str(), maxLen);
	buf[maxLen - 1] = '\0';
	bool changed = ImGui::InputText(label, buf, maxLen);
	if (changed) {
		str = buf;
	}
	return changed;
}

} // namespace blot
