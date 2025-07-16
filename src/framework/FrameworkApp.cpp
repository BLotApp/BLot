#include "FrameworkApp.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "rendering/Renderer.h"

FrameworkApp::FrameworkApp(std::unique_ptr<IWork> work)
    : m_work(std::move(work))
    , m_ecsManager(std::make_unique<ECSManager>())
    , m_renderer(createRenderer(RendererType::Blend2D)) // or OpenGL, configurable
    , m_addonManager(std::make_unique<AddonManager>())
{}

void FrameworkApp::run() {
    m_work->setup(this);
    bool running = true;
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (running) {
        auto now = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration<float>(now - lastTime).count();
        lastTime = now;
        m_work->update(deltaTime);
        m_work->draw();
        // For now, just run once and exit (for Blend2D sample)
        running = false;
    }
} 