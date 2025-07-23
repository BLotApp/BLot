#pragma once

// Standard library
#include <functional>
#include <memory>

// Project headers
#include "Blend2DRenderer.h"
#include "core/IAddon.h"
#include "rendering/U_rendering.h"

namespace blot {

class bxBlend2d : public blot::IAddon {
  public:
	bxBlend2d();
	~bxBlend2d() override;

	bool init() override;
	void setup() override;
	void update(float deltaTime) override;
	void draw() override;
	void cleanup() override;

	// Static registration function for the renderer factory
	static void registerRendererFactory();
};
} // namespace blot
