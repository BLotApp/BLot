#pragma once

#include "../PropertyReflection.h"
#include <string>
#include <vector>

namespace blot {
namespace components {

struct Style {
	// Fill color (RGBA)
	float fillR = 1.0f, fillG = 1.0f, fillB = 1.0f, fillA = 1.0f;

	// Stroke color (RGBA)
	float strokeR = 0.0f, strokeG = 0.0f, strokeB = 0.0f, strokeA = 1.0f;
	float strokeWidth = 1.0f;

	bool hasFill = true;
	bool hasStroke = true;

	// Stroke style
	enum class StrokeCap { Butt, Square, Round };
	enum class StrokeJoin { Miter, Bevel, Round };
	StrokeCap strokeCap = StrokeCap::Butt;
	StrokeJoin strokeJoin = StrokeJoin::Miter;

	// Dash pattern
	std::vector<float> dashPattern;
	float dashOffset = 0.0f;

	// Utility methods
	void setFillColor(float r, float g, float b, float a = 1.0f) {
		fillR = r;
		fillG = g;
		fillB = b;
		fillA = a;
	}

	void setStrokeColor(float r, float g, float b, float a = 1.0f) {
		strokeR = r;
		strokeG = g;
		strokeB = b;
		strokeA = a;
	}

	void setStrokeWidth(float width) { strokeWidth = width; }

	void setStrokeCap(StrokeCap cap) { strokeCap = cap; }

	void setStrokeJoin(StrokeJoin join) { strokeJoin = join; }

	void setDashPattern(const std::vector<float> &pattern,
						float offset = 0.0f) {
		dashPattern = pattern;
		dashOffset = offset;
	}

	std::vector<sProp> GetProperties() {
		return {{0, "Fill R", EPT_FLOAT, &fillR},
				{1, "Fill G", EPT_FLOAT, &fillG},
				{2, "Fill B", EPT_FLOAT, &fillB},
				{3, "Fill A", EPT_FLOAT, &fillA},
				{4, "Stroke R", EPT_FLOAT, &strokeR},
				{5, "Stroke G", EPT_FLOAT, &strokeG},
				{6, "Stroke B", EPT_FLOAT, &strokeB},
				{7, "Stroke A", EPT_FLOAT, &strokeA},
				{8, "Stroke Width", EPT_FLOAT, &strokeWidth},
				{9, "Has Fill", EPT_BOOL, &hasFill},
				{10, "Has Stroke", EPT_BOOL, &hasStroke},
				{11, "Dash Offset", EPT_FLOAT, &dashOffset}};
	}
};

} // namespace components
} // namespace blot
