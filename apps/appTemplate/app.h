#pragma once
#include "core/U_core.h"

class AppTemplate : public blot::IApp {
  public:
	AppTemplate() {
		window().width = 1024;
		window().height = 768;
		window().title = "App Template";
		// window().fullscreen = true;
	}
	void setup() override {
		spdlog::info("App Template setup!");
		if (auto engine = getEngine()) {
			engine->setClearColor(0.4f, 0.4f, 0.4f, 1.0f);
		}
	}
	void update(float) override {}
	void draw() override {

		// What would you like to draw today?
	}
};
