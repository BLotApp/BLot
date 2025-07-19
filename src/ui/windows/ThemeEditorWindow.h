#pragma once

#include <memory>
#include <string>
#include "Window.h"

// Forward declarations
namespace blot {
class UIManager;
}

namespace blot {

class ThemeEditorWindow : public Window {
  public:
	ThemeEditorWindow(const std::string &title = "Theme Editor",
					  Flags flags = Flags::None);
	virtual ~ThemeEditorWindow() = default;

	void setUIManager(UIManager *uiManager);
	void renderContents() override;

  private:
	UIManager *m_uiManager = nullptr;
	void renderThemeEditor();
};

} // namespace blot
