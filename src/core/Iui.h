#pragma once

#include "core/ISettings.h"

namespace blot {

class BlotEngine; // forward

class Iui : public ISettings {
  public:
	virtual ~Iui() = default;

	// Called once after window/context creation
	virtual void init() = 0;

	// Per-frame input processing (keyboard/mouse, etc.)
	virtual void handleInput() = 0;

	// Per-frame state update ( dock-space, animations … )
	virtual void update() = 0;

	// Per-frame rendering of the UI
	virtual void render() = 0;

	// Called once before engine shutdown
	virtual void shutdown() = 0;

	// Give the UI manager a pointer back to the engine (optional)
	virtual void setBlotEngine(BlotEngine *engine) = 0;
};

} // namespace blot
