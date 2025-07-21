#include "bxNodeEditor.h"
#include <iostream>
#include "windows/NodeEditorWindow.h"

bxNodeEditor::bxNodeEditor()
	: blot::AddonBase("bxNodeEditor", "0.1.0"), m_initialized(false),
	  m_time(0.0f) {
	setDescription("Node Editor");
	setAuthor("Your Name");
	setLicense("MIT");
}

bxNodeEditor::~bxNodeEditor() { cleanup(); }

bool bxNodeEditor::init() {
	this->log("Initializing bxNodeEditor addon");

	// Initialize addon-specific resources
	m_initialized = true;

	// Create and register the Node Editor window
	if (auto mui = dynamic_cast<Mui *>(engine->getUIManager())) {
		auto wnd = std::make_shared<blot::NodeEditorWindow>("Node Editor");
		mui->getWindowManager()->createWindow("NodeEditor", wnd);
	}

	this->log("bxNodeEditor addon initialized successfully");
	return true;
}

void bxNodeEditor::setup() { this->log("Setting up bxNodeEditor addon"); }

void bxNodeEditor::update(float deltaTime) { m_time += deltaTime; }

void bxNodeEditor::draw() {
	// Draw addon-specific UI or graphics
	// This is called during the main draw loop
}

void bxNodeEditor::cleanup() {
	if (m_initialized) {
		this->log("Cleaning up bxNodeEditor addon");
		m_initialized = false;
	}
}
