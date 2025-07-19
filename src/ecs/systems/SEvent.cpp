#include "SEvent.h"
#include <iostream>

namespace blot {
namespace ecs {

SEvent::SEvent(entt::registry &registry) : m_registry(registry) {}

void SEvent::registerEvent(const std::string &eventId,
								std::function<void(const CEvent &)> handler) {
	m_eventHandlers[eventId].push_back(handler);
}

void SEvent::unregisterEvent(const std::string &eventId) {
	auto it = m_eventHandlers.find(eventId);
	if (it != m_eventHandlers.end()) {
		m_eventHandlers.erase(it);
	}
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<void()> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<void(int)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(
	const std::string &actionId,
	std::function<void(const std::string &)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<void(uint32_t)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(
	const std::string &actionId,
	std::function<void(const std::string &, bool)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(
	const std::string &actionId,
	std::function<bool(const std::string &)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(
	const std::string &actionId,
	std::function<std::vector<std::string>()> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<std::string()> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(
	const std::string &actionId,
	std::function<std::vector<std::pair<std::string, std::string>>()> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<void(bool)> action) {
	m_actions[actionId] = action;
}

void SEvent::registerAction(const std::string &actionId,
								 std::function<bool()> action) {
	m_actions[actionId] = action;
}

void SEvent::emitEvent(const CEvent &event) {
	// Process global event handlers
	auto it = m_eventHandlers.find(event.actionId);
	if (it != m_eventHandlers.end()) {
		for (auto &handler : it->second) {
			handler(event);
		}
	}

	// Process ECS event listeners
	processCEventListeners(event);
}

void SEvent::emitEvent(EET_EventType type, const std::string &actionId) {
	CEvent event(type, actionId);
	emitEvent(event);
}

void SEvent::emitEvent(EET_EventType type, const std::string &actionId,
							const std::any &data) {
	CEvent event(type, actionId, data);
	emitEvent(event);
}

void SEvent::triggerAction(const std::string &actionId) {
	auto it = m_actions.find(actionId);
	if (it != m_actions.end()) {
		try {
			if (auto action =
					std::any_cast<std::function<void()>>(&it->second)) {
				(*action)();
			}
		} catch (const std::bad_any_cast &) {
			std::cerr << "Action '" << actionId << "' is not a void() function"
					  << std::endl;
		}
	}
}

void SEvent::triggerAction(const std::string &actionId,
								const std::any &data) {
	auto it = m_actions.find(actionId);
	if (it != m_actions.end()) {
		// Try to find a matching action signature
		try {
			if (auto action =
					std::any_cast<std::function<void(const std::any &)>>(
						&it->second)) {
				(*action)(data);
				return;
			}
		} catch (const std::bad_any_cast &) {
		}

		try {
			if (auto action =
					std::any_cast<std::function<void(int)>>(&it->second)) {
				if (data.type() == typeid(int)) {
					(*action)(std::any_cast<int>(data));
					return;
				}
			}
		} catch (const std::bad_any_cast &) {
		}

		try {
			if (auto action =
					std::any_cast<std::function<void(const std::string &)>>(
						&it->second)) {
				if (data.type() == typeid(std::string)) {
					(*action)(std::any_cast<std::string>(data));
					return;
				}
			}
		} catch (const std::bad_any_cast &) {
		}

		std::cerr << "Action '" << actionId
				  << "' cannot handle the provided data type" << std::endl;
	}
}

entt::entity
SEvent::createListener(const std::string &eventId,
							std::function<void(const CEvent &)> callback) {
	auto entity = m_registry.create();
	m_registry.emplace<CEventListener>(entity, eventId, callback);
	return entity;
}

void SEvent::removeListener(entt::entity entity) {
	if (m_registry.valid(entity) && m_registry.all_of<CEventListener>(entity)) {
		m_registry.destroy(entity);
	}
}

void SEvent::createEmitter(entt::entity entity,
								const std::vector<std::string> &events) {
	if (m_registry.valid(entity)) {
		m_registry.emplace<CEventEmitter>(entity, events);
	}
}

void SEvent::update() {
	// Clean up inactive listeners
	cleanupInactiveListeners();

	// Process any pending events (could be extended for event queuing)
}

bool SEvent::hasAction(const std::string &actionId) const {
	return m_actions.find(actionId) != m_actions.end();
}

std::vector<std::string> SEvent::getRegisteredActions() const {
	std::vector<std::string> actions;
	for (const auto &[id, _] : m_actions) {
		actions.push_back(id);
	}
	return actions;
}

void SEvent::processCEventListeners(const CEvent &event) {
	auto view = m_registry.view<CEventListener>();
	for (auto entity : view) {
		auto &listener = view.get<CEventListener>(entity);
		if (listener.active && listener.eventId == event.actionId) {
			listener.callback(event);
		}
	}
}

void SEvent::cleanupInactiveListeners() {
	auto view = m_registry.view<CEventListener>();
	std::vector<entt::entity> toRemove;

	for (auto entity : view) {
		auto &listener = view.get<CEventListener>(entity);
		if (!listener.active) {
			toRemove.push_back(entity);
		}
	}

	for (auto entity : toRemove) {
		m_registry.destroy(entity);
	}
}

} // namespace ecs
} // namespace blot
