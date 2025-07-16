#pragma once
#include <memory>
#include "IWork.h"
#include "ecs/ECSManager.h"
#include "rendering/Renderer.h"
#include "addons/AddonManager.h"

class FrameworkApp {
public:
    FrameworkApp(std::unique_ptr<IWork> work);
    void run();
    ECSManager* getECSManager() { return m_ecsManager.get(); }
    IRenderer* getRenderer() { return m_renderer.get(); }
    AddonManager* getAddonManager() { return m_addonManager.get(); }

private:
    std::unique_ptr<IWork> m_work;
    std::unique_ptr<ECSManager> m_ecsManager;
    std::unique_ptr<IRenderer> m_renderer;
    std::unique_ptr<AddonManager> m_addonManager;
}; 