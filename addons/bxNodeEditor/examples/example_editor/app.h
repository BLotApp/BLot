#pragma once
#include "bxImGui.h"
#include "bxNodeEditor.h"
#include "core/U_core.h"
#include "windows/NodeEditorWindow.h"

class ExampleEditorApp : public blot::IApp {
  public:
	ExampleEditorApp() {
		window().width = 1280;
		window().height = 720;
		window().title = "Example Node Editor";
	}

	void setup() override {
		// Init engine
		getEngine()->init("Example Node Editor", 0.1f);
		// Register addons
		if (auto am = getAddonManager()) {
			am->registerAddon(std::make_shared<bxImGui>());
			am->registerAddon(std::make_shared<bxNodeEditor>());
			am->initAll();
		}
		// Show NodeEditor window centered
		if (auto mui = dynamic_cast<blot::Mui *>(getUIManager())) {
			mui->setWindowVisibility("NodeEditor", true);
		}
	}

	void update(float) override {}
	void draw() override {}
};
