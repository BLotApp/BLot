#pragma once

#include "Mui.h"
#include "core/IAddon.h"

namespace blot {
class bxImGui : public blot::IAddon {
  public:
	bxImGui();
	bool init() override;
	void setup() override {}
	void update(float dt) override;
	void draw() override {}
	void cleanup() override;

  private:
	Mui *m_ui = nullptr;
};
} // namespace blot
