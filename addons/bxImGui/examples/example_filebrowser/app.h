#pragma once
#include <spdlog/spdlog.h>
#include "ImGuiFileBrowser.h"
#include "bxImGui.h"
#include "core/U_core.h"

class FileBrowserApp : public blot::IApp {
  public:
	FileBrowserApp() {
		window().width = 1280;
		window().height = 720;
		window().title = "ImGui FileBrowser Demo";
	}

	void setup() override {
		getEngine()->init("FileBrowser Demo", 0.1f);
		if (auto am = getAddonManager()) {
			am->registerAddon(std::make_shared<bxImGui>());
			am->initAll();
		}
		if (auto mui = dynamic_cast<blot::Mui *>(getUIManager())) {
			mui->setWindowVisibilityAll(false);
		}
		fileDialog.SetTitle("Select a File");
	}

	void draw() override {
		if (ImGui::Begin("File Browser Demo")) {
			if (ImGui::Button("Open Dialog")) {
				fileDialog.Open();
			}
		}
		ImGui::End();
		fileDialog.Display();
		if (fileDialog.HasSelected()) {
			spdlog::info("User chose: {}", fileDialog.GetSelected().string());
			fileDialog.ClearSelected();
		}
	}

  private:
	ImGui::FileBrowser fileDialog;
};
