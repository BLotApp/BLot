#pragma once
#include "ImGuizmo.h"
#include "bxImGui.h"
#include "core/U_core.h"

class ImGuizmoApp : public blot::IApp {
  public:
	ImGuizmoApp() {
		window().width = 1280;
		window().height = 720;
		window().title = "ImGuizmo Demo";
	}

	void setup() override {
		getEngine()->init("ImGuizmo Demo", 0.1f);
		if (auto am = getAddonManager()) {
			am->registerAddon(std::make_shared<bxImGui>());
			am->initAll();
		}
		if (auto mui = dynamic_cast<blot::Mui *>(getUIManager())) {
			mui->setWindowVisibilityAll(false);
		}
	}

	void draw() override {
		// Start ImGui frame already done by Mui.
		ImGuizmo::BeginFrame();
		ImGuizmo::SetOrthographic(false);
		ImGuizmo::SetDrawlist();
		float view[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		float proj[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		float matrix[16] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
		ImGuizmo::Manipulate(view, proj, ImGuizmo::TRANSLATE, ImGuizmo::LOCAL,
							 matrix);
	}
};
