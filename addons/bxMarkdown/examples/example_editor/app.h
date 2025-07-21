#pragma once
#include <cstring>
#include "bxImGui.h"
#include "bxMarkdown.h"
#include "core/U_core.h"
#include "imgui_markdown.h"

static ImGui::MarkdownConfig mdConfig; // default config

class MarkdownApp : public blot::IApp {
  public:
	MarkdownApp() {
		window().width = 960;
		window().height = 600;
		window().title = "Markdown Demo";
	}

	void setup() override {
		getEngine()->init("Markdown Demo", 0.1f);
		if (auto am = getAddonManager()) {
			am->registerAddon(std::make_shared<bxImGui>());
			am->registerAddon(std::make_shared<bxMarkdown>());
			am->initAll();
		}
	}

	void draw() override {
		if (ImGui::Begin("Markdown Viewer")) {
			static const char *md =
				R"(# Hello Markdown
This **text** is rendered with `imgui_markdown`.

* bullet 1
* bullet 2
)";
			ImGui::Markdown(md, std::strlen(md), mdConfig);
		}
		ImGui::End();
	}
};
