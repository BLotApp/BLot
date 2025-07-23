#include "bxMarkdown.h"

bxMarkdown::bxMarkdown() : blot::IAddon("bxMarkdown", "0.1.0") {
	setDescription("Markdown support for Blot");
	setAuthor("Your Name");
	setLicense("MIT");
}

bool bxMarkdown::init() {
	this->log("Initializing bxMarkdown addon");

	// Initialize addon-specific resources
	m_initialized = true;

	this->log("bxMarkdown addon initialized successfully");
	return true;
}
