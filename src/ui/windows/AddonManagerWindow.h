#pragma once

#include "Window.h"
#include <memory>

class AddonManager;

namespace blot {

class AddonManagerWindow : public Window {
public:
    AddonManagerWindow(const std::string& title = "Addon Manager", Flags flags = Flags::None);
    virtual ~AddonManagerWindow() = default;

    void setAddonManager(std::shared_ptr<AddonManager> addonManager);
    void renderContents() override;

private:
    std::shared_ptr<AddonManager> m_addonManager;
    void renderAddonManager();
};

} // namespace blot 