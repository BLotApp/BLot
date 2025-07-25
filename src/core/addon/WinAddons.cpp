#include "WinAddons.h"
#include "IAddon.h"
#include "MAddon.h"

namespace blot {

WinAddons::WinAddons(const std::string &title) : m_title(title) {}

void WinAddons::setAddonManager(blot::MAddon *addonManager) {
	m_addonManager = addonManager;
}

// IWindow interface implementation
void WinAddons::show() { m_visible = true; }

void WinAddons::hide() { m_visible = false; }

void WinAddons::close() { m_visible = false; }

bool WinAddons::isVisible() const { return m_visible; }

void WinAddons::setTitle(const std::string &title) { m_title = title; }

std::string WinAddons::getTitle() const { return m_title; }

void WinAddons::setFlags(WindowFlags flags) { m_flags = flags; }

WindowFlags WinAddons::getFlags() const { return m_flags; }

void WinAddons::setPosition(int x, int y) {
	m_x = x;
	m_y = y;
}

void WinAddons::getPosition(int &x, int &y) const {
	x = m_x;
	y = m_y;
}

void WinAddons::setSize(int width, int height) {
	m_width = width;
	m_height = height;
}

void WinAddons::getSize(int &width, int &height) const {
	width = m_width;
	height = m_height;
}

void WinAddons::renderContents() {
	// This is now a pure data interface
	// UI rendering will be handled by the addon system
}

// Data access methods for UI addons
std::vector<std::shared_ptr<IAddon>> WinAddons::getAddons() const {
	if (!m_addonManager)
		return {};
	return m_addonManager->getAllAddons();
}

bool WinAddons::isAddonEnabled(const std::string &addonName) const {
	if (!m_addonManager)
		return false;
	auto addon = m_addonManager->getAddon(addonName);
	return addon && addon->isEnabled();
}

void WinAddons::enableAddon(const std::string &addonName) {
	if (m_addonManager)
		m_addonManager->enableAddon(addonName);
}

void WinAddons::disableAddon(const std::string &addonName) {
	if (m_addonManager)
		m_addonManager->disableAddon(addonName);
}

void WinAddons::reloadAddons() {
	if (m_addonManager) {
		m_addonManager->cleanupAll();
		m_addonManager->initAll();
	}
}

} // namespace blot
