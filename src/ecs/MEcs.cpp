#include "ecs/MEcs.h"
#include <algorithm>
#include <entt/entt.hpp>
#include <iostream>
#include "core/ISettings.h"
#include "ecs/components/CAnimation.h"
#include "ecs/components/CDrawing.h"
#include "ecs/components/CNode.h"
#include "ecs/components/CParameter.h"
#include "ecs/components/CScript.h"
#include "ecs/components/CSelection.h"
#include "ecs/components/CShape.h"
#include "ecs/components/CDrawStyle.h"
#include "ecs/components/CTransform.h"
#include "ecs/systems/SCanvas.h"
#include "ecs/systems/SShapeRendering.h"

namespace blot {

MEcs::MEcs() {
	// Initialize event system
	m_eventSystem = std::make_unique<blot::ecs::SEvent>(m_registry);
}

MEcs::~MEcs() { clear(); }

entt::entity MEcs::createEntity(const std::string &name) {
	auto entity = m_registry.create();
	m_entities.push_back(entity);

	if (!name.empty()) {
		m_namedEntities[name] = entity;
	}

	return entity;
}

void MEcs::destroyEntity(entt::entity entity) {
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

entt::entity MEcs::findEntity(const std::string &name) {
	auto it = m_namedEntities.find(name);
	if (it != m_namedEntities.end()) {
		return it->second;
	}
	return entt::null;
}

void MEcs::updateSystems(float deltaTime) {
	// Update event system
	if (m_eventSystem) {
		m_eventSystem->update();
	}

	updateAnimationSystem(deltaTime);
	updateScriptSystem(deltaTime);
	updateParameterSystem();
}

void MEcs::renderSystems() { renderShapeSystem(); }

void MEcs::connectParameters(entt::entity source,
								   const std::string &sourceParam,
								   entt::entity target,
								   const std::string &targetParam) {
	if (!m_registry.valid(source) || !m_registry.valid(target)) {
		return;
	}

	if (m_registry.all_of<blot::ecs::CParameter>(source) &&
		m_registry.all_of<blot::ecs::CParameter>(target)) {
		auto &sourceParamComp =
			m_registry.get<blot::ecs::CParameter>(source);
		auto &targetParamComp =
			m_registry.get<blot::ecs::CParameter>(target);

		sourceParamComp.isConnected = true;
		sourceParamComp.connectedTo = target;
		targetParamComp.isConnected = true;
		targetParamComp.connectedTo = source;

		// Transfer value
		targetParamComp.value = sourceParamComp.value;
	}
}

void MEcs::disconnectParameters(entt::entity source,
									  const std::string &sourceParam) {
	if (!m_registry.valid(source)) {
		return;
	}

	if (m_registry.all_of<blot::ecs::CParameter>(source)) {
		auto &sourceParamComp =
			m_registry.get<blot::ecs::CParameter>(source);
		sourceParamComp.isConnected = false;
		sourceParamComp.connectedTo = entt::null;
	}
}

void MEcs::createNode(const std::string &nodeType, float x, float y) {
	auto entity = createEntity("node_" + nodeType);

	// Add transform component
	blot::ecs::CTransform transform;
	transform.position.x = x;
	transform.position.y = y;
	addComponent<blot::ecs::CTransform>(entity, transform);

	// Add node component
	blot::ecs::CNodeComponent node;
	node.type = blot::ecs::CNodeType::Custom; // or map nodeType string to
													// enum if needed
	node.name = nodeType;
	addComponent<blot::ecs::CNodeComponent>(entity, node);

	// Add default parameters based on node type
	if (nodeType == "oscillator") {
		blot::ecs::CParameter freq;
		freq.name = "frequency";
		freq.value = 440.0f;
		freq.minValue = 20.0f;
		freq.maxValue = 20000.0f;
		addComponent<blot::ecs::CParameter>(entity, freq);

		blot::ecs::CParameter amp;
		amp.name = "amplitude";
		amp.value = 0.5f;
		amp.minValue = 0.0f;
		amp.maxValue = 1.0f;
		addComponent<blot::ecs::CParameter>(entity, amp);
	} else if (nodeType == "filter") {
		blot::ecs::CParameter cutoff;
		cutoff.name = "cutoff";
		cutoff.value = 1000.0f;
		cutoff.minValue = 20.0f;
		cutoff.maxValue = 20000.0f;
		addComponent<blot::ecs::CParameter>(entity, cutoff);

		blot::ecs::CParameter resonance;
		resonance.name = "resonance";
		resonance.value = 0.5f;
		resonance.minValue = 0.0f;
		resonance.maxValue = 1.0f;
		addComponent<blot::ecs::CParameter>(entity, resonance);
	}
}

void MEcs::connectNodes(entt::entity sourceNode,
							  const std::string &output,
							  entt::entity targetNode,
							  const std::string &input) {
	if (!m_registry.valid(sourceNode) || !m_registry.valid(targetNode)) {
		return;
	}

	if (m_registry.all_of<blot::ecs::CNodeComponent>(sourceNode) &&
		m_registry.all_of<blot::ecs::CNodeComponent>(targetNode)) {
		auto &sourceNodeComp =
			m_registry.get<blot::ecs::CNodeComponent>(sourceNode);
		auto &targetNodeComp =
			m_registry.get<blot::ecs::CNodeComponent>(targetNode);

		// Add output to source node (using pins)
		auto outputs = sourceNodeComp.getOutputs();
		if (std::find_if(outputs.begin(), outputs.end(),
						 [&](const blot::ecs::CNodePin &pin) {
							 return pin.name == output;
						 }) == outputs.end()) {
			// Add a new output pin if not found
			sourceNodeComp.pins.push_back(
				{output, "float", false, true, 0.0f, "Output"});
		}
		// Add input to target node (using pins)
		auto inputs = targetNodeComp.getInputs();
		if (std::find_if(inputs.begin(), inputs.end(),
						 [&](const blot::ecs::CNodePin &pin) {
							 return pin.name == input;
						 }) == inputs.end()) {
			// Add a new input pin if not found
			targetNodeComp.pins.push_back(
				{input, "float", true, false, 0.0f, "Input"});
		}
	}
}

void MEcs::clear() {
	m_registry.clear();
	m_namedEntities.clear();
	m_entities.clear();
}

size_t MEcs::getEntityCount() const { return m_entities.size(); }

std::vector<entt::entity> MEcs::getAllEntities() const {
	return m_entities;
}

void MEcs::updateAnimationSystem(float deltaTime) {
	auto view =
		m_registry
			.view<blot::ecs::CAnimation, blot::ecs::CTransform>();

	for (auto entity : view) {
		auto &animation = view.get<blot::ecs::CAnimation>(entity);
		auto &transform = view.get<blot::ecs::CTransform>(entity);

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
			case blot::ecs::CAnimation::EaseIn:
				easedProgress = progress * progress;
				break;
			case blot::ecs::CAnimation::EaseOut:
				easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);
				break;
			case blot::ecs::CAnimation::EaseInOut:
				easedProgress =
					progress < 0.5f
						? 2.0f * progress * progress
						: 1.0f - 2.0f * (1.0f - progress) * (1.0f - progress);
				break;
			case blot::ecs::CAnimation::Bounce:
				easedProgress = 1.0f - std::cos(progress * 3.14159f * 2.0f) *
										   std::exp(-progress * 3.0f);
				break;
			case blot::ecs::CAnimation::Elastic:
				easedProgress = std::sin(progress * 3.14159f * 2.0f) *
								std::exp(-progress * 2.0f);
				break;
			}

			// Apply to transform (example: animate position)
			transform.position.x = easedProgress * 100.0f; // Simple example
		}
	}
}

void MEcs::updateScriptSystem(float /*deltaTime*/) {
	// Scripting engine removed for now
}

void MEcs::renderShapeSystem() {
	// Rendering now handled by runShapeRenderingSystem using an external
	// IRenderer.
}

void MEcs::updateParameterSystem() {
	auto view = m_registry.view<blot::ecs::CParameter>();

	for (auto entity : view) {
		auto &parameter = view.get<blot::ecs::CParameter>(entity);

		// Update connected parameters
		if (parameter.isConnected && m_registry.valid(parameter.connectedTo)) {
			if (m_registry.all_of<blot::ecs::CParameter>(
					parameter.connectedTo)) {
				auto &connectedParam =
					m_registry.get<blot::ecs::CParameter>(
						parameter.connectedTo);
				connectedParam.value = parameter.value;
			}
		}
	}
}

void MEcs::runCanvasSystems(MRendering *renderingManager,
								  float deltaTime) {
	blot::ecs::SCanvasUpdate(*this, renderingManager, deltaTime);
}

void MEcs::runCanvasRenderSystem(MRendering *renderingManager,
									   entt::entity activeCanvasId) {
	blot::ecs::SCanvasRender(*this, renderingManager, activeCanvasId);
}

void MEcs::runShapeRenderingSystem(std::shared_ptr<IRenderer> renderer) {
	blot::ecs::SShapeRendering(*this, renderer);
}

blot::json MEcs::getSettings() const {
	blot::json j;
	// Collect settings for all entities and their components
	j["entities"] = blot::json::array();
	for (auto entity : m_entities) {
		blot::json entityJson;
		entityJson["id"] = static_cast<uint32_t>(entity);
		// For each component type, use property reflection if available
		// TODO: Serialize CTransform once json conversion helper
		// exists if (m_registry.all_of<blot::ecs::CTransform>(entity)) {
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

void MEcs::setSettings(const blot::json &settings) {
	clear();
	if (!settings.contains("entities"))
		return;
	for (const auto &entityJson : settings["entities"]) {
		std::string name = entityJson.value("name", "");
		entt::entity entity = createEntity(name);
		// For each component type, use property reflection if available
		if (entityJson.contains("Transform")) {
			blot::ecs::CTransform comp;
			// Use your property reflection or manual assignment here
			// For example:
			// comp.x = entityJson["Transform"]["x"];
			// ...
			addComponent<blot::ecs::CTransform>(entity, comp);
		}
		// Repeat for other component types as needed...
	}
}

} // namespace blot
