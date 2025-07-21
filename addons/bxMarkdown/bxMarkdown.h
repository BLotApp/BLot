#pragma once
#include "core/AddonBase.h"
#include "imgui_markdown.h"

class bxMarkdown : public AddonBase {
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
