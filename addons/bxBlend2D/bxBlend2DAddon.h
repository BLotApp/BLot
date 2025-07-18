#pragma once

// Standard library
#include <memory>
#include <functional>

// Project headers
#include "rendering/rendering.h"
#include "addons/bxBlend2D/Blend2DRenderer.h"
#include "core/AddonBase.h"

namespace blot {
// bxBlend2DAddon: Registers the Blend2D renderer with the engine
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