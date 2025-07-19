#pragma once

#include <memory>
#include <string>
#include "imgui.h"

class ImGuiRenderer {
  public:
	ImGuiRenderer();
	~ImGuiRenderer();

	void init();
	void render();
	void setCustomFont(const std::string &fontPath, int fontSize = 16);

	// ImGui integration helpers
	void pushCustomFont();
	void popCustomFont();

  private:
	bool m_useCustomFont;
	std::string m_customFontPath;
	int m_customFontSize;

	// ImGui font atlas
	ImFont *m_customImGuiFont;
	ImFontAtlas *m_fontAtlas;
};
