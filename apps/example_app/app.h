#pragma once

#include "core/U_core.h"

class ExampleApp : public blot::IApp {
  public:
	ExampleApp() {
		window().width = 1024;
		window().height = 768;
		window().title = "Example App";
	}

	void setup() override;
	void update(float deltaTime) override;
	void draw() override;
};
