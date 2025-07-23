#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "core/IManager.h"
#include "core/ISettings.h"
#include "ecs/systems/SEvent.h"
#include "imgui.h"
#include "rendering/IRenderer.h"

// Forward declarations
class bxScriptEngine;

namespace blot {
class Canvas;
class MRendering;
class MEcs : public IManager, public ISettings {
  public:
	MEcs();
	~MEcs();
	void init() override {}
	void shutdown() override {}

	// Entity management
	entt::entity createEntity(const std::string &name = "");
	void destroyEntity(entt::entity entity);
	entt::entity findEntity(const std::string &name);

	// Component management
	template <typename T>
	T &addComponent(entt::entity entity, const T &component = T{});

	template <typename T> T &getComponent(entt::entity entity);

	template <typename T> bool hasComponent(entt::entity entity);

	template <typename T> void removeComponent(entt::entity entity);

	// System management
	void updateSystems(MRendering *renderingManager, float deltaTime);
	void renderSystems();

	// Query systems
	template <typename... Components> auto view();

	// Parameter patching
	void connectParameters(entt::entity source, const std::string &sourceParam,
						   entt::entity target, const std::string &targetParam);
	void disconnectParameters(entt::entity source,
							  const std::string &sourceParam);

	// Node editor integration
	void createNode(const std::string &nodeType, float x, float y);
	void connectNodes(entt::entity sourceNode, const std::string &output,
					  entt::entity targetNode, const std::string &input);

	// Shape creation helpers
	entt::entity createRectangle(float x, float y, float width, float height);
	entt::entity createEllipse(float x, float y, float width, float height);
	entt::entity createLine(float x1, float y1, float x2, float y2);
	entt::entity createPolygon(float x, float y, float radius, int sides);
	entt::entity createStar(float x, float y, float outerRadius,
							float innerRadius, int points);

	// Drawing system
	void startDrawing(entt::entity entity, const ImVec2 &startPos);
	void updateDrawing(entt::entity entity, const ImVec2 &currentPos);
	void finishDrawing(entt::entity entity, const ImVec2 &endPos);
	void selectEntity(entt::entity entity, bool multiSelect = false);
	void clearSelection();

	// Utility functions
	void clear();
	size_t getEntityCount() const;
	std::vector<entt::entity> getAllEntities() const;

	// Integration with other systems
	// void setGraphics(std::shared_ptr<Graphics> graphics); // Removed, not
	// needed

	void runCanvasSystems(MRendering *renderingManager, float deltaTime);
	void runCanvasRenderSystem(MRendering *renderingManager,
							   entt::entity activeCanvasId);
	void runShapeRenderingSystem(std::shared_ptr<IRenderer> renderer);

	// Event system access
	ecs::SEvent &getEventSystem() { return *m_eventSystem; }
	const ecs::SEvent &getEventSystem() const { return *m_eventSystem; }

	// ISettings interface
	json getSettings() const override;
	void setSettings(const json &settings) override;

  private:
	entt::registry m_registry;
	std::unordered_map<std::string, entt::entity> m_namedEntities;
	std::vector<entt::entity> m_entities;

	// System references
	// std::shared_ptr<Canvas> m_canvas; // Removed, not needed

	// Event system
	std::unique_ptr<ecs::SEvent> m_eventSystem;

	// Systems
	void updateAnimationSystem(float deltaTime);
	void updateScriptSystem(float deltaTime);
	void renderShapeSystem();
	void updateParameterSystem();
};

// Template implementations
template <typename T>
T &MEcs::addComponent(entt::entity entity, const T &component) {
	return m_registry.emplace<T>(entity, component);
}

template <typename T> T &MEcs::getComponent(entt::entity entity) {
	return m_registry.get<T>(entity);
}

template <typename T> bool MEcs::hasComponent(entt::entity entity) {
	return m_registry.all_of<T>(entity);
}

template <typename T> void MEcs::removeComponent(entt::entity entity) {
	m_registry.remove<T>(entity);
}

template <typename... Components> auto MEcs::view() {
	return m_registry.view<Components...>();
}
} // namespace blot
