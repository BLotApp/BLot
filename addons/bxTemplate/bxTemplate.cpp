#include "bxTemplate.h"

bxTemplate::bxTemplate() : blot::IAddon("bxTemplate", "1.0.0") {
	setDescription("Template addon for BLot creative coding");
	setAuthor("Your Name");
	setLicense("MIT");
}

bxTemplate::~bxTemplate() { cleanup(); }

bool bxTemplate::init() {
	this->log("Initializing bxTemplate addon");

	// Initialize addon-specific resources
	// m_initialized is now handled by IAddon

	this->log("bxTemplate addon initialized successfully");
	return true;
}

void bxTemplate::setup() {
	this->log("Setting up bxTemplate addon");

	// Example: Set up default parameters
	// setParameter("speed", 1.0f);
	// setParameter("amplitude", 100.0f);
	// setParameter("frequency", 0.5f);
}

void bxTemplate::update(float deltaTime) {
	// Update addon logic here
	// Example: Update parameters based on time
	// float speed = getParameter("speed");
	// float amplitude = getParameter("amplitude");
	// float frequency = getParameter("frequency");

	// You can trigger events
	// this->triggerEvent("update");
}

void bxTemplate::draw() {
	// Draw addon-specific UI or graphics
	// This is called during the main draw loop
}

void bxTemplate::cleanup() {
	this->log("Cleaning up bxTemplate addon");
	// Addon-specific cleanup
	// m_initialized is now handled by IAddon
}

// Parameter methods are now inherited from IAddon
// No need to reimplement setParameter, getParameter, onParameterChanged
