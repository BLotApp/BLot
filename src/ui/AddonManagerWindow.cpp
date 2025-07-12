#include "AddonManagerWindow.h"
#include "addons/AddonManager.h"
#include <imgui.h>

namespace blot {

AddonManagerWindow::AddonManagerWindow(const std::string& title, Flags flags)
    : Window(title, flags) {}

// Window interface implementations
void AddonManagerWindow::show() { m_state.isOpen = true; }
void AddonManagerWindow::hide() { m_state.isOpen = false; }
void AddonManagerWindow::close() { m_state.isOpen = false; }
void AddonManagerWindow::toggle() { m_state.isOpen = !m_state.isOpen; }
bool AddonManagerWindow::isOpen() const { return m_state.isOpen; }
bool AddonManagerWindow::isVisible() const { return m_state.isOpen; }
bool AddonManagerWindow::isFocused() const { return ImGui::IsWindowFocused(); }
bool AddonManagerWindow::isHovered() const { return ImGui::IsWindowHovered(); }
bool AddonManagerWindow::isDragging() const { return false; }
bool AddonManagerWindow::isResizing() const { return false; }
void AddonManagerWindow::setPosition(const ImVec2& pos) { /* TODO: Implement if needed */ }
void AddonManagerWindow::setSize(const ImVec2& size) { /* TODO: Implement if needed */ }
void AddonManagerWindow::setMinSize(const ImVec2& minSize) { /* TODO: Implement if needed */ }
void AddonManagerWindow::setMaxSize(const ImVec2& maxSize) { /* TODO: Implement if needed */ }
void AddonManagerWindow::setFlags(Window::Flags flags) { m_state.flags = flags; }
Window::Flags AddonManagerWindow::getFlags() const { return m_state.flags; }
const std::string& AddonManagerWindow::getTitle() const { return m_title; }
void AddonManagerWindow::setTitle(const std::string& title) { m_title = title; }

void AddonManagerWindow::setAddonManager(std::shared_ptr<AddonManager> addonManager) {
    m_addonManager = addonManager;
}

void AddonManagerWindow::render() {
    begin();
    if (isOpen() && m_addonManager) {
        renderAddonManager();
    }
    end();
}

void AddonManagerWindow::renderAddonManager() {
    if (!m_addonManager) return;
    if (ImGui::BeginTabBar("AddonManagerTabs")) {
        if (ImGui::BeginTabItem("Addons")) {
            // Render addon list
            auto addons = m_addonManager->getAllAddons();
            ImGui::Text("Available Addons (%zu)", addons.size());
            ImGui::Separator();
            for (const auto& addon : addons) {
                if (!addon) continue;
                ImGui::PushID(addon->getName().c_str());
                bool enabled = addon->isEnabled();
                if (ImGui::Checkbox(addon->getName().c_str(), &enabled)) {
                    if (enabled) m_addonManager->enableAddon(addon->getName());
                    else m_addonManager->disableAddon(addon->getName());
                }
                if (ImGui::IsItemHovered()) {
                    ImGui::SetTooltip("%s\nVersion: %s\nAuthor: %s",
                        addon->getDescription().c_str(),
                        addon->getVersion().c_str(),
                        addon->getAuthor().c_str());
                }
                ImGui::PopID();
            }
            ImGui::Separator();
            if (ImGui::Button("Reload Addons")) {
                m_addonManager->cleanupAll();
                m_addonManager->initAll();
            }
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Details")) {
            // Optionally render details (could be expanded)
            ImGui::Text("Select an addon to see details.");
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
}

} // namespace blot 