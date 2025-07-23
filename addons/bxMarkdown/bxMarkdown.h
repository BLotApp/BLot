#pragma once
#include "core/IAddon.h"
// #include "imgui_markdown.h"  // Temporarily commented out - imgui_markdown
// library not properly configured

class bxMarkdown : public blot::IAddon {
  public:
	bxMarkdown();
	bool init();
	void setup();
	void update(float deltaTime);
	void draw();
	void cleanup();

  private:
	bool m_initialized;
	float m_time;
};
