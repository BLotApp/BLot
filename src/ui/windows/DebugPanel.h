#pragma once

#include <string>

namespace blot {
class MEcs;
}
#include <imgui.h>
#include "Window.h"

namespace blot {

class DebugPanel : public Window {
  public:
	DebugPanel(const std::string &title = "Debug Panel###DebugPanel",
			   Flags flags = Flags::None);
	virtual ~DebugPanel() = default;

	// Debug functionality
	void setECSManager(MEcs *ecs) { m_ecs = ecs; }
	void setDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }

	void renderContents() override;

  private:
	MEcs *m_ecs = nullptr;
	float m_deltaTime = 0.0f;

	// Debug methods
	void renderDebugInfo();
	void renderClearShapesButton();
	void renderEntityInfo();
	void renderPerformanceInfo();
};

} // namespace blot
