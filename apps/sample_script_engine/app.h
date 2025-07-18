#pragma once
#include "core/core.h"
#include <memory>
#include <iostream>
#include "addons/bxScriptEngine/bxScriptEngine.h"

class SampleScriptEngineApp : public IApp {
public:
    SampleScriptEngineApp() : m_scriptEngine(std::make_shared<bxScriptEngine>()) {}
    void setup(BlotEngine*) override {
        std::cout << "SampleScriptEngineApp setup!" << std::endl;
        std::string code = "void setup() {}\nvoid draw() {}";
        m_scriptEngine->runCode(code);
    }
    void update(float) override {
        m_scriptEngine->updateScript(1.0f / 60.0f);
    }
    void draw() override {}
    void configureWindow(WindowSettings& settings) override;
private:
    std::shared_ptr<bxScriptEngine> m_scriptEngine;
}; 