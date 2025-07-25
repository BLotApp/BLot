#pragma once
#include "core/U_core.h"

class GradientTestApp : public blot::IApp {
  public:
	GradientTestApp() {
		window().width = 800;
		window().height = 600;
		window().title = "Gradient Test App";
		m_time = 0.0f;
	}

	void setup() override;
	void update(float deltaTime) override;
	void draw() override;

  private:
	float m_time;
};
