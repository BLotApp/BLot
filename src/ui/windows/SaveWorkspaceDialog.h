#pragma once

#include <functional>
#include <string>
#include "Window.h"

namespace blot {

class SaveWorkspaceDialog : public Window {
  public:
	SaveWorkspaceDialog(const std::string &title,
						Window::Flags flags = Window::Flags::None);
	~SaveWorkspaceDialog() = default;

	// Callbacks
	void setSaveCallback(std::function<void(const std::string &)> callback) {
		m_saveCallback = callback;
	}
	void setCancelCallback(std::function<void()> callback) {
		m_cancelCallback = callback;
	}

	// Set initial workspace name
	void setInitialWorkspaceName(const std::string &name) {
		m_workspaceName = name;
	}

	void renderContents() override;

  private:
	std::function<void(const std::string &)> m_saveCallback;
	std::function<void()> m_cancelCallback;
	std::string m_workspaceName;
	char m_nameBuffer[256];
	bool m_shouldClose = false;
};

} // namespace blot
