#pragma once

#include <memory>
#include "Window.h"

namespace blot {

class AddonManager;

class AddonManagerWindow : public Window {
  public:
	AddonManagerWindow(const std::string &title = "Addon Manager",
					   Flags flags = Flags::None);
	virtual ~AddonManagerWindow() = default;

	void setAddonManager(blot::AddonManager *addonManager);
	void renderContents() override;

  private:
	blot::AddonManager *m_addonManager = nullptr;
	void renderAddonManager();
};

} // namespace blot
