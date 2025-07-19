#pragma once

#include "ecs/components/CEvent.h"

namespace blot {
namespace ecs {

class SEvent {
  public:
	SEvent(entt::registry &registry);
	~SEvent() = default;

	// Event registration
	void registerEvent(const std::string &eventId,
					   std::function<void(const CEvent &)> handler);
	void unregisterEvent(const std::string &eventId);

	// Action registration (for menu/UI actions)
	void registerAction(const std::string &actionId,
						std::function<void()> action);
	void registerAction(const std::string &actionId,
						std::function<void(int)> action);
	void registerAction(const std::string &actionId,
						std::function<void(const std::string &)> action);
	void registerAction(const std::string &actionId,
						std::function<void(uint32_t)> action);
	void registerAction(const std::string &actionId,
						std::function<void(const std::string &, bool)> action);
	void registerAction(const std::string &actionId,
						std::function<bool(const std::string &)> action);
	void registerAction(const std::string &actionId,
						std::function<std::vector<std::string>()> action);
	void registerAction(const std::string &actionId,
						std::function<std::string()> action);
	void registerAction(
		const std::string &actionId,
		std::function<std::vector<std::pair<std::string, std::string>>()>
			action);
	void registerAction(const std::string &actionId,
						std::function<void(bool)> action);
	void registerAction(const std::string &actionId,
						std::function<bool()> action);

	// Event emission
	void emitEvent(const CEvent &event);
	void emitEvent(EET_EventType type, const std::string &actionId);
	void emitEvent(EET_EventType type, const std::string &actionId,
				   const std::any &data);

	// Action triggering
	void triggerAction(const std::string &actionId);
	void triggerAction(const std::string &actionId, const std::any &data);

	// ECS integration
	entt::entity createListener(const std::string &eventId,
								std::function<void(const CEvent &)> callback);
	void removeListener(entt::entity entity);
	void createEmitter(entt::entity entity,
					   const std::vector<std::string> &events);

	// System update
	void update();

	// Query actions
	bool hasAction(const std::string &actionId) const;
	std::vector<std::string> getRegisteredActions() const;

	// Template for type-safe action registration
	template <typename... Args>
	void registerAction(const std::string &actionId,
						std::function<void(Args...)> action);

	template <typename T>
	T triggerActionWithResult(const std::string &actionId) {
		auto it = m_actions.find(actionId);
		if (it != m_actions.end()) {
			try {
				auto func = std::any_cast<std::function<T()>>(&it->second);
				if (func)
					return (*func)();
			} catch (const std::bad_any_cast &) {
			}
		}
		return T{};
	}

	template <typename T, typename Arg>
	T triggerActionWithResult(const std::string &actionId, const Arg &arg) {
		auto it = m_actions.find(actionId);
		if (it != m_actions.end()) {
			try {
				auto func = std::any_cast<std::function<T(Arg)>>(&it->second);
				if (func)
					return (*func)(arg);
			} catch (const std::bad_any_cast &) {
			}
		}
		return T{};
	}

  private:
	entt::registry &m_registry;

	// Event handlers
	std::unordered_map<std::string,
					   std::vector<std::function<void(const CEvent &)>>>
		m_eventHandlers;

	// Action handlers (using std::any for type flexibility)
	std::unordered_map<std::string, std::any> m_actions;

	// Helper methods
	void processCEventListeners(const CEvent &event);
	void cleanupInactiveListeners();
};

// Template implementation
template <typename... Args>
void SEvent::registerAction(const std::string &actionId,
								 std::function<void(Args...)> action) {
	m_actions[actionId] = action;
}

} // namespace ecs
} // namespace blot
