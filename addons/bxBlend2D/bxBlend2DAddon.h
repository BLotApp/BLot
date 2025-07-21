#pragma once

// Standard library
#include <functional>
#include <memory>

// Project headers
#include "Blend2DRenderer.h"
#include "core/AddonBase.h"
#include "rendering/U_rendering.h"

namespace blot {

class bxBlend2DAddon : public AddonBase {
  public:
	bxBlend2DAddon();
	~bxBlend2DAddon() override;

	bool init() override;
	void setup() override;
	void update(float deltaTime) override;
	void draw() override;
	void cleanup() override;

	// Static registration function for the renderer factory
	static void registerRendererFactory();
};
} // namespace blot
