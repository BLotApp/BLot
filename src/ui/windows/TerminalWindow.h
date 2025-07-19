#pragma once

#include <imgui.h>
#include <string>
#include <vector>
#include "Window.h"

namespace blot {

class TerminalWindow : public Window {
  public:
	TerminalWindow(const std::string &title = "Terminal###Terminal",
				   Flags flags = Flags::None);
	virtual ~TerminalWindow() = default;

	// Terminal functionality
	void addLog(const std::string &message);
	void clearLog();
	void executeCommand(const std::string &command);
	void renderContents() override;

  private:
	std::vector<std::string> m_logHistory;
	char m_inputBuffer[1024] = {0};
	bool m_scrollToBottom = true;

	// Terminal methods
	void renderInput();
	void renderLogHistory();
	void processCommand(const std::string &command);
};

} // namespace blot
