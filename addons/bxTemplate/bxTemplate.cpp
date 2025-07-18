#include "bxTemplate.h"
#include <iostream>

bxTemplate::bxTemplate()
    : AddonBase("bxTemplate", "1.0.0")
    , m_initialized(false)
    , m_time(0.0f)
{
    setDescription("Template addon for BLot creative coding");
    setAuthor("Your Name");
    setLicense("MIT");
}

bxTemplate::~bxTemplate() {
    cleanup();
}

bool bxTemplate::init() {
    log("Initializing bxTemplate addon");
    
    // Initialize addon-specific resources
    m_initialized = true;
    
    log("bxTemplate addon initialized successfully");
    return true;
}

void bxTemplate::setup() {
    log("Setting up bxTemplate addon");
    
    // Set up default parameters
    setParameter("speed", 1.0f);
    setParameter("amplitude", 100.0f);
    setParameter("frequency", 0.5f);
}

void bxTemplate::update(float deltaTime) {
    m_time += deltaTime;
    
    // Update addon logic here
    // Example: Update parameters based on time
    float speed = getParameter("speed");
    float amplitude = getParameter("amplitude");
    float frequency = getParameter("frequency");
    
    // You can trigger events here
    triggerEvent("update");
}

void bxTemplate::draw() {
    // Draw addon-specific UI or graphics
    // This is called during the main draw loop
}

void bxTemplate::cleanup() {
    if (m_initialized) {
        log("Cleaning up bxTemplate addon");
        m_initialized = false;
    }
}

void bxTemplate::setParameter(const std::string& name, float value) {
    m_parameters[name] = value;
    
    // Trigger callback if registered
    auto it = m_callbacks.find(name);
    if (it != m_callbacks.end()) {
        it->second(value);
    }
    
    log("Parameter " + name + " set to " + std::to_string(value));
}

float bxTemplate::getParameter(const std::string& name) const {
    auto it = m_parameters.find(name);
    if (it != m_parameters.end()) {
        return it->second;
    }
    return 0.0f;
}

void bxTemplate::onParameterChanged(const std::string& name, std::function<void(float)> callback) {
    m_callbacks[name] = callback;
} 