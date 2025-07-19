#pragma once

#include <functional>
#include "Window.h"

namespace blot {

class ImGuiWindow : public Window {
  public:
	ImGuiWindow(const std::string &title, Flags flags = Flags::None);
	virtual ~ImGuiWindow() = default;

	// Custom render callback
	void setRenderCallback(std::function<void()> callback);
	void renderContents() override;

  protected:
	// Custom render callback
	std::function<void()> m_renderCallback;
};

} // namespace blot
