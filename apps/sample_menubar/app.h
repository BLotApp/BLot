#pragma once
#include "core/U_core.h"

class SampleMenubarApp : public blot::IApp {
  public:
	SampleMenubarApp() {
		window().width = 1280;
		window().height = 720;
		window().title = "Sample Menubar";
	}

	void setup() override { getEngine()->init("Sample Menubar", 0.1f); }

	void update(float) override {}
	void draw() override {

		// What would you like to draw today?
	}
};
