#include "ImGuiWindow.h"
#include <imgui.h>

namespace blot {

ImGuiWindow::ImGuiWindow(const std::string &title, Flags flags)
	: Window(title, flags) {}

void ImGuiWindow::renderContents() {
	if (m_renderCallback) {
		m_renderCallback();
	} else {
		ImGui::Text("ImGuiWindow base class - override renderContents() in "
					"derived class");
	}
}

void ImGuiWindow::setRenderCallback(std::function<void()> callback) {
	m_renderCallback = callback;
}

} // namespace blot
