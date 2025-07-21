#pragma once

#include "core/AddonBase.h"
#include "ui/Mui.h"

namespace blot {
class bxImGui : public AddonBase {
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
