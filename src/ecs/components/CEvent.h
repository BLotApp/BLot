#pragma once

#include <any>
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace blot {
namespace ecs {

// Event types
enum class EET_EventType {
	MenuAction,
	WindowAction,
	ThemeAction,
	WorkspaceAction,
	CanvasAction,
	DebugAction,
	Custom
};

struct CEvent {
	EET_EventType type;
	std::string actionId;
	std::any data;
	entt::entity source = entt::null;
	entt::entity target = entt::null;

	CEvent(EET_EventType t, const std::string &id) : type(t), actionId(id) {}
	CEvent(EET_EventType t, const std::string &id, const std::any &d)
		: type(t), actionId(id), data(d) {}
};

// SEvent listener component
struct CEventListener {
	std::string eventId;
	std::function<void(const CEvent &)> callback;
	bool active = true;

	CEventListener(const std::string &id,
				   std::function<void(const CEvent &)> cb)
		: eventId(id), callback(cb) {}
};

// Event emitter component
struct CEventEmitter {
	std::vector<std::string> events;
	bool active = true;
};

} // namespace ecs
} // namespace blot
