#pragma once
#include <memory>
#include <string>
#include <vector>
#include "MWindow.h"
#include "Window.h"

namespace blot {

class WindowManagerPanel : public Window {
  public:
	WindowManagerPanel(const std::string &title, MWindow *windowManager,
					   Flags flags = Flags::None);

  protected:
	void renderContents() override;

  private:
	MWindow *m_windowManager;
};

} // namespace blot
