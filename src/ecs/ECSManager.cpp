#include "ecs/ECSManager.h"
#include "core/ISettings.h"
#include "ecs/components/AnimationComponent.h"
#include "ecs/components/DrawingComponent.h"
#include "ecs/components/NodeComponent.h"
#include "ecs/components/ParameterComponent.h"
#include "ecs/components/ScriptComponent.h"
#include "ecs/components/SelectionComponent.h"
#include "ecs/components/ShapeComponent.h"
#include "ecs/components/StyleComponent.h"
#include "ecs/components/TransformComponent.h"
#include "ecs/systems/CanvasSystems.h"
#include "ecs/systems/ShapeRenderingSystem.h"
#include <algorithm>
#include <entt/entt.hpp>
#include <iostream>

namespace blot {

ECSManager::ECSManager() {
	// Initialize event system
	m_eventSystem = std::make_unique<blot::systems::EventSystem>(m_registry);
}

ECSManager::~ECSManager() { clear(); }

entt::entity ECSManager::createEntity(const std::string &name) {
	auto entity = m_registry.create();
	m_entities.push_back(entity);

	if (!name.empty()) {
		m_namedEntities[name] = entity;
	}

	return entity;
}

void ECSManager::destroyEntity(entt::entity entity) {
	// Remove from named entities
	for (auto it = m_namedEntities.begin(); it != m_namedEntities.end(); ++it) {
		if (it->second == entity) {
			m_namedEntities.erase(it);
			break;
		}
	}

	// Remove from entities list
	auto it = std::find(m_entities.begin(), m_entities.end(), entity);
	if (it != m_entities.end()) {
		m_entities.erase(it);
	}

	m_registry.destroy(entity);
}

entt::entity ECSManager::findEntity(const std::string &name) {
	auto it = m_namedEntities.find(name);
	if (it != m_namedEntities.end()) {
		return it->second;
	}
	return entt::null;
}

void ECSManager::updateSystems(float deltaTime) {
	// Update event system
	if (m_eventSystem) {
		m_eventSystem->update();
	}

	updateAnimationSystem(deltaTime);
	updateScriptSystem(deltaTime);
	updateParameterSystem();
}

void ECSManager::renderSystems() { renderShapeSystem(); }

void ECSManager::connectParameters(entt::entity source,
								   const std::string &sourceParam,
								   entt::entity target,
								   const std::string &targetParam) {
	if (!m_registry.valid(source) || !m_registry.valid(target)) {
		return;
	}

	if (m_registry.all_of<blot::components::Parameter>(source) &&
		m_registry.all_of<blot::components::Parameter>(target)) {
		auto &sourceParamComp =
			m_registry.get<blot::components::Parameter>(source);
		auto &targetParamComp =
			m_registry.get<blot::components::Parameter>(target);

		sourceParamComp.isConnected = true;
		sourceParamComp.connectedTo = target;
		targetParamComp.isConnected = true;
		targetParamComp.connectedTo = source;

		// Transfer value
		targetParamComp.value = sourceParamComp.value;
	}
}

void ECSManager::disconnectParameters(entt::entity source,
									  const std::string &sourceParam) {
	if (!m_registry.valid(source)) {
		return;
	}

	if (m_registry.all_of<blot::components::Parameter>(source)) {
		auto &sourceParamComp =
			m_registry.get<blot::components::Parameter>(source);
		sourceParamComp.isConnected = false;
		sourceParamComp.connectedTo = entt::null;
	}
}

void ECSManager::createNode(const std::string &nodeType, float x, float y) {
	auto entity = createEntity("node_" + nodeType);

	// Add transform component
	blot::components::Transform transform;
	transform.position.x = x;
	transform.position.y = y;
	addComponent<blot::components::Transform>(entity, transform);

	// Add node component
	blot::components::NodeComponent node;
	node.type = blot::components::NodeType::Custom; // or map nodeType string to
													// enum if needed
	node.name = nodeType;
	addComponent<blot::components::NodeComponent>(entity, node);

	// Add default parameters based on node type
	if (nodeType == "oscillator") {
		blot::components::Parameter freq;
		freq.name = "frequency";
		freq.value = 440.0f;
		freq.minValue = 20.0f;
		freq.maxValue = 20000.0f;
		addComponent<blot::components::Parameter>(entity, freq);

		blot::components::Parameter amp;
		amp.name = "amplitude";
		amp.value = 0.5f;
		amp.minValue = 0.0f;
		amp.maxValue = 1.0f;
		addComponent<blot::components::Parameter>(entity, amp);
	} else if (nodeType == "filter") {
		blot::components::Parameter cutoff;
		cutoff.name = "cutoff";
		cutoff.value = 1000.0f;
		cutoff.minValue = 20.0f;
		cutoff.maxValue = 20000.0f;
		addComponent<blot::components::Parameter>(entity, cutoff);

		blot::components::Parameter resonance;
		resonance.name = "resonance";
		resonance.value = 0.5f;
		resonance.minValue = 0.0f;
		resonance.maxValue = 1.0f;
		addComponent<blot::components::Parameter>(entity, resonance);
	}
}

void ECSManager::connectNodes(entt::entity sourceNode,
							  const std::string &output,
							  entt::entity targetNode,
							  const std::string &input) {
	if (!m_registry.valid(sourceNode) || !m_registry.valid(targetNode)) {
		return;
	}

	if (m_registry.all_of<blot::components::NodeComponent>(sourceNode) &&
		m_registry.all_of<blot::components::NodeComponent>(targetNode)) {
		auto &sourceNodeComp =
			m_registry.get<blot::components::NodeComponent>(sourceNode);
		auto &targetNodeComp =
			m_registry.get<blot::components::NodeComponent>(targetNode);

		// Add output to source node (using pins)
		auto outputs = sourceNodeComp.getOutputs();
		if (std::find_if(outputs.begin(), outputs.end(),
						 [&](const blot::components::NodePin &pin) {
							 return pin.name == output;
						 }) == outputs.end()) {
			// Add a new output pin if not found
			sourceNodeComp.pins.push_back(
				{output, "float", false, true, 0.0f, "Output"});
		}
		// Add input to target node (using pins)
		auto inputs = targetNodeComp.getInputs();
		if (std::find_if(inputs.begin(), inputs.end(),
						 [&](const blot::components::NodePin &pin) {
							 return pin.name == input;
						 }) == inputs.end()) {
			// Add a new input pin if not found
			targetNodeComp.pins.push_back(
				{input, "float", true, false, 0.0f, "Input"});
		}
	}
}

void ECSManager::clear() {
	m_registry.clear();
	m_namedEntities.clear();
	m_entities.clear();
}

size_t ECSManager::getEntityCount() const { return m_entities.size(); }

std::vector<entt::entity> ECSManager::getAllEntities() const {
	return m_entities;
}

void ECSManager::updateAnimationSystem(float deltaTime) {
	auto view =
		m_registry
			.view<blot::components::Animation, blot::components::Transform>();

	for (auto entity : view) {
		auto &animation = view.get<blot::components::Animation>(entity);
		auto &transform = view.get<blot::components::Transform>(entity);

		if (animation.isPlaying) {
			animation.currentTime += deltaTime;

			if (animation.currentTime >= animation.duration) {
				if (animation.loop) {
					animation.currentTime = 0.0f;
				} else {
					animation.isPlaying = false;
				}
			}

			// Apply animation curve
			float progress = animation.currentTime / animation.duration;
			float easedProgress = progress; // Default linear

			switch (animation.curve) {
			case blot::components::Animation::EaseIn:
				easedProgress = progress * progress;
				break;
			case blot::components::Animation::EaseOut:
				easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);
				break;
			case blot::components::Animation::EaseInOut:
				easedProgress =
					progress < 0.5f
						? 2.0f * progress * progress
						: 1.0f - 2.0f * (1.0f - progress) * (1.0f - progress);
				break;
			case blot::components::Animation::Bounce:
				easedProgress = 1.0f - std::cos(progress * 3.14159f * 2.0f) *
										   std::exp(-progress * 3.0f);
				break;
			case blot::components::Animation::Elastic:
				easedProgress = std::sin(progress * 3.14159f * 2.0f) *
								std::exp(-progress * 2.0f);
				break;
			}

			// Apply to transform (example: animate position)
			transform.position.x = easedProgress * 100.0f; // Simple example
		}
	}
}

void ECSManager::updateScriptSystem(float /*deltaTime*/) {
	// Scripting engine removed for now
}

void ECSManager::renderShapeSystem() {
	// Rendering now handled by runShapeRenderingSystem using an external
	// IRenderer.
}

void ECSManager::updateParameterSystem() {
	auto view = m_registry.view<blot::components::Parameter>();

	for (auto entity : view) {
		auto &parameter = view.get<blot::components::Parameter>(entity);

		// Update connected parameters
		if (parameter.isConnected && m_registry.valid(parameter.connectedTo)) {
			if (m_registry.all_of<blot::components::Parameter>(
					parameter.connectedTo)) {
				auto &connectedParam =
					m_registry.get<blot::components::Parameter>(
						parameter.connectedTo);
				connectedParam.value = parameter.value;
			}
		}
	}
}

void ECSManager::runCanvasSystems(RenderingManager *renderingManager,
								  float deltaTime) {
	blot::systems::CanvasUpdateSystem(*this, renderingManager, deltaTime);
}

void ECSManager::runCanvasRenderSystem(RenderingManager *renderingManager,
									   entt::entity activeCanvasId) {
	blot::systems::CanvasRenderSystem(*this, renderingManager, activeCanvasId);
}

void ECSManager::runShapeRenderingSystem(std::shared_ptr<IRenderer> renderer) {
	blot::systems::ShapeRenderingSystem(*this, renderer);
}

blot::json ECSManager::getSettings() const {
	blot::json j;
	// Collect settings for all entities and their components
	j["entities"] = blot::json::array();
	for (auto entity : m_entities) {
		blot::json entityJson;
		entityJson["id"] = static_cast<uint32_t>(entity);
		// For each component type, use property reflection if available
		// TODO: Serialize Transform component once json conversion helper
		// exists if (m_registry.all_of<blot::components::Transform>(entity)) {
		// ... } Repeat for other component types as needed... Optionally, add
		// entity name if available
		for (const auto &[name, ent] : m_namedEntities) {
			if (ent == entity) {
				entityJson["name"] = name;
				break;
			}
		}
		j["entities"].push_back(entityJson);
	}
	return j;
}

void ECSManager::setSettings(const blot::json &settings) {
	clear();
	if (!settings.contains("entities"))
		return;
	for (const auto &entityJson : settings["entities"]) {
		std::string name = entityJson.value("name", "");
		entt::entity entity = createEntity(name);
		// For each component type, use property reflection if available
		if (entityJson.contains("Transform")) {
			blot::components::Transform comp;
			// Use your property reflection or manual assignment here
			// For example:
			// comp.x = entityJson["Transform"]["x"];
			// ...
			addComponent<blot::components::Transform>(entity, comp);
		}
		// Repeat for other component types as needed...
	}
}

} // namespace blot
