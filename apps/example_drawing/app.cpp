#include "app.h"
#include <cmath>
#include <spdlog/spdlog.h>

void GradientTestApp::setup() { spdlog::info("GradientTestApp setup"); }

void GradientTestApp::update(float deltaTime) { m_time += deltaTime; }

void GradientTestApp::draw() {
	// Clear with a dark background
	graphics().clear(0.1f, 0.1f, 0.1f, 1.0f);

	// Create a simple linear gradient
	std::vector<GradientStop> stops = {
		{0.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)}, // Red
		{0.5f, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)}, // Green
		{1.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)}  // Blue
	};

	graphics().setLinearGradient(0, 0, window().width, window().height, stops);
	graphics().drawRect(0, 0, window().width, window().height);

	// Create a radial gradient
	std::vector<GradientStop> radialStops = {
		{0.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)}, // White
		{1.0f, glm::vec4(0.0f, 0.0f, 0.0f, 0.0f)}  // Transparent
	};

	graphics().setRadialGradient(window().width * 0.5f, window().height * 0.5f,
								 100, radialStops);
	graphics().drawCircle(window().width * 0.5f, window().height * 0.5f, 200);

	// Clear gradient for next operations
	graphics().clearGradient();
}
