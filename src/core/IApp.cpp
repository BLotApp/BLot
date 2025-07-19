#include "core/IApp.h"
#include "core/BlotEngine.h"

namespace blot {

ECSManager* IApp::getECSManager() const {
    return m_engine ? m_engine->getECSManager() : nullptr;
}

RenderingManager* IApp::getRenderingManager() const {
    return m_engine ? m_engine->getRenderingManager() : nullptr;
}

CanvasManager* IApp::getCanvasManager() const {
    return m_engine ? m_engine->getCanvasManager() : nullptr;
}

UIManager* IApp::getUIManager() const {
    return m_engine ? m_engine->getUIManager() : nullptr;
}

AddonManager* IApp::getAddonManager() const {
    return m_engine ? m_engine->getAddonManager() : nullptr;
}

SettingsManager* IApp::getSettingsManager() const {
    return m_engine ? m_engine->getSettings() : nullptr;
}

} // namespace blot 