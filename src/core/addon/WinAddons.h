#pragma once

#include <memory>
#include <string>
#include <vector>
#include "core/IWindow.h"

namespace blot {

class MAddon;
class IAddon;

class WinAddons : public IWindow {
  public:
	WinAddons(const std::string &title = "Addons");
	virtual ~WinAddons() = default;

	void setAddonManager(blot::MAddon *addonManager);

	// IWindow interface implementation
	void show() override;
	void hide() override;
	void close() override;
	bool isVisible() const override;
	void setTitle(const std::string &title) override;
	std::string getTitle() const override;
	void setFlags(WindowFlags flags) override;
	WindowFlags getFlags() const override;
	void setPosition(int x, int y) override;
	void getPosition(int &x, int &y) const override;
	void setSize(int width, int height) override;
	void getSize(int &width, int &height) const override;
	void renderContents() override;

	// Data access methods for UI addons
	std::vector<std::shared_ptr<IAddon>> getAddons() const;
	bool isAddonEnabled(const std::string &addonName) const;
	void enableAddon(const std::string &addonName);
	void disableAddon(const std::string &addonName);
	void reloadAddons();

  private:
	blot::MAddon *m_addonManager = nullptr;
	std::string m_title;
	WindowFlags m_flags = WindowFlags::None;
	bool m_visible = false;
	int m_x = 100, m_y = 100;
	int m_width = 400, m_height = 300;
};

} // namespace blot
