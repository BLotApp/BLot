#pragma once

#include <memory>
#include <string>
#include "Window.h"

// Forward declarations
namespace blot {
class Mui;
}

namespace blot {

class ThemeEditorWindow : public Window {
  public:
	ThemeEditorWindow(const std::string &title = "Theme Editor",
					  Flags flags = Flags::None);
	virtual ~ThemeEditorWindow() = default;

	void setUIManager(Mui *uiManager);
	void renderContents() override;

  private:
	Mui *m_uiManager = nullptr;
	void renderThemeEditor();
};

} // namespace blot
