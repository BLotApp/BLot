#pragma once

#include <imgui.h>
#include <string>
#include "ImGuiWindow.h"

namespace blot {

class TextureViewerWindow : public ImGuiWindow {
  public:
	TextureViewerWindow(
		const std::string &title = "Texture Viewer###TextureViewer",
		Flags flags = Flags::NoScrollbar | Flags::NoCollapse);
	virtual ~TextureViewerWindow() = default;

	void setTexture(unsigned int textureId, int width, int height);
	void clearTexture();
	void handleMouseInput();
	ImVec2 getTextureMousePos() const;
	bool isMouseInsideTexture() const;
	ImVec2 getTextureSize() const;
	std::string getName() const;
	std::string getTitle() const;
	void renderContents() override;

  private:
	void drawTexture();
	ImVec2 convertToTextureCoordinates(const ImVec2 &screenPos) const;
	unsigned int m_textureId = 0;
	int m_textureWidth = 0;
	int m_textureHeight = 0;
	bool m_hasTexture = false;
	ImVec2 m_texturePos = ImVec2(0, 0);
	ImVec2 m_textureSize = ImVec2(0, 0);
	ImVec2 m_textureEnd = ImVec2(0, 0);
	ImVec2 m_textureMousePos = ImVec2(0, 0);
	bool m_mouseInsideTexture = false;
};

} // namespace blot
