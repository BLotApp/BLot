#pragma once
#include <iostream>
#include <memory>
#include "addons/bxScriptEngine/bxScriptEngine.h"
#include "core/U_core.h"

namespace blot {
class BlotEngine;
class IApp;
} // namespace blot

class SimpleConsoleApp : public blot::IApp {
  public:
	SimpleConsoleApp() : m_scriptEngine(std::make_shared<bxScriptEngine>()) {}
	void setup(blot::BlotEngine *) override {
		std::cout << "SimpleConsoleApp setup!" << std::endl;
		const std::string code = "void setup() {}\nvoid draw() {}";
		m_scriptEngine->runCode(code);
	}
	void update(float) override { m_scriptEngine->updateScript(1.0f / 60.0f); }
	void draw() override {}
	void configureWindow(WindowSettings &) override {}

  private:
	std::shared_ptr<bxScriptEngine> m_scriptEngine;
};
