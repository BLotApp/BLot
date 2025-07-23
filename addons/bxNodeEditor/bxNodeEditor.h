#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "core/IAddon.h"

class bxNodeEditor : public blot::IAddon {
  public:
	bxNodeEditor();
	~bxNodeEditor();

	bool init();
	void setup();
	void update(float deltaTime);
	void draw();
	void cleanup();

  private:
	bool m_initialized;
	float m_time;
};
