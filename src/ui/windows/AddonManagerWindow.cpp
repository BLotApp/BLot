#include "AddonManagerWindow.h"
#include "core/AddonBase.h"
#include "core/AddonManager.h"
#include <imgui.h>

namespace blot {

AddonManagerWindow::AddonManagerWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

void AddonManagerWindow::setAddonManager(blot::AddonManager *addonManager) {
	m_addonManager = addonManager;
}

void AddonManagerWindow::renderContents() {
	if (!m_addonManager)
		return;
	renderAddonManager();
}

void AddonManagerWindow::renderAddonManager() {
	if (!m_addonManager)
		return;
	if (ImGui::BeginTabBar("AddonManagerTabs")) {
		if (ImGui::BeginTabItem("Addons")) {
			// Render addon list
			auto addons = m_addonManager->getAllAddons();
			ImGui::Text("Available Addons (%zu)", addons.size());
			ImGui::Separator();
			for (const auto &addon : addons) {
				if (!addon)
					continue;
				ImGui::PushID(addon->getName().c_str());
				bool enabled = addon->isEnabled();
				if (ImGui::Checkbox(addon->getName().c_str(), &enabled)) {
					if (enabled)
						m_addonManager->enableAddon(addon->getName());
					else
						m_addonManager->disableAddon(addon->getName());
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
