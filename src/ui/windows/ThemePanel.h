#pragma once

#include <imgui.h>
#include <memory>
#include "Window.h"

namespace blot {

class ThemePanel : public Window {
  public:
	ThemePanel(const std::string &title = "Theme Panel###ThemePanel",
			   Flags flags = Flags::None);
	virtual ~ThemePanel() = default;

	void renderContents() override;

  private:
	// Theme methods
	void renderRandomThemeButton();
	void renderColorPresets();
	void renderThemeControls();
};

} // namespace blot
