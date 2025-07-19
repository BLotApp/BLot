#pragma once

#include <memory>
#include "Window.h"

namespace blot {

class MAddon;

class AddonManagerWindow : public Window {
  public:
	AddonManagerWindow(const std::string &title = "Addon Manager",
					   Flags flags = Flags::None);
	virtual ~AddonManagerWindow() = default;

	void setAddonManager(blot::MAddon *addonManager);
	void renderContents() override;

  private:
	blot::MAddon *m_addonManager = nullptr;
	void renderAddonManager();
};

} // namespace blot
