#pragma once
#include "../WindowManager.h"
#include "Window.h"
#include <memory>
#include <string>
#include <vector>

namespace blot {

class WindowManagerPanel : public Window {
  public:
	WindowManagerPanel(const std::string &title, WindowManager *windowManager,
					   Flags flags = Flags::None);

  protected:
	void renderContents() override;

  private:
	WindowManager *m_windowManager;
};

} // namespace blot
