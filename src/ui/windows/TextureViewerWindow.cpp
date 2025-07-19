#include "TextureViewerWindow.h"
#include "imgui.h"

namespace blot {

TextureViewerWindow::TextureViewerWindow(const std::string &title, Flags flags)
	: ImGuiWindow(title, flags) {}

void TextureViewerWindow::setTexture(unsigned int textureId, int width,
									 int height) {
	m_textureId = textureId;
	m_textureWidth = width;
	m_textureHeight = height;
	m_hasTexture = (textureId != 0);
}

void TextureViewerWindow::clearTexture() {
	m_textureId = 0;
	m_textureWidth = 0;
	m_textureHeight = 0;
	m_hasTexture = false;
}

void TextureViewerWindow::handleMouseInput() {
	if (!isOpen()) {
		return;
	}
	m_texturePos = ImGui::GetCursorScreenPos();
	m_textureSize = ImGui::GetContentRegionAvail();
	m_textureEnd = ImVec2(m_texturePos.x + m_textureSize.x,
						  m_texturePos.y + m_textureSize.y);
	ImVec2 mousePos = ImGui::GetMousePos();
	m_mouseInsideTexture =
		(mousePos.x >= m_texturePos.x && mousePos.x <= m_textureEnd.x &&
		 mousePos.y >= m_texturePos.y && mousePos.y <= m_textureEnd.y);
	if (m_mouseInsideTexture) {
		m_textureMousePos = convertToTextureCoordinates(mousePos);
	}
}

ImVec2 TextureViewerWindow::getTextureMousePos() const {
	return m_textureMousePos;
}

bool TextureViewerWindow::isMouseInsideTexture() const {
	return m_mouseInsideTexture;
}

ImVec2 TextureViewerWindow::getTextureSize() const { return m_textureSize; }

std::string TextureViewerWindow::getName() const { return m_title; }

std::string TextureViewerWindow::getTitle() const { return m_title; }

void TextureViewerWindow::renderContents() {
	drawTexture();
	handleMouseInput();
}

void TextureViewerWindow::drawTexture() {
	if (m_hasTexture && m_textureId != 0) {
		ImVec2 availableSize = ImGui::GetContentRegionAvail();
		float aspectRatio = static_cast<float>(m_textureWidth) /
							static_cast<float>(m_textureHeight);
		float displayWidth = availableSize.x;
		float displayHeight = availableSize.x / aspectRatio;
		if (displayHeight > availableSize.y) {
			displayHeight = availableSize.y;
			displayWidth = availableSize.y * aspectRatio;
		}
		ImVec2 displaySize(displayWidth, displayHeight);
		float xOffset = (availableSize.x - displaySize.x) * 0.5f;
		float yOffset = (availableSize.y - displaySize.y) * 0.5f;
		ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + xOffset,
								   ImGui::GetCursorPos().y + yOffset));
		ImGui::Image(
			reinterpret_cast<void *>(static_cast<uintptr_t>(m_textureId)),
			displaySize, ImVec2(0, 0), ImVec2(1, 1));
		if (ImGui::IsWindowHovered()) {
			ImGui::SetTooltip("Texture: %dx%d", m_textureWidth,
							  m_textureHeight);
		}
	} else {
		ImVec2 availableSize = ImGui::GetContentRegionAvail();
		ImVec2 centerPos =
			ImVec2(ImGui::GetCursorPos().x + availableSize.x * 0.5f - 50,
				   ImGui::GetCursorPos().y + availableSize.y * 0.5f - 10);
		ImGui::SetCursorPos(centerPos);
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "No Texture");
	}
}

ImVec2 TextureViewerWindow::convertToTextureCoordinates(
	const ImVec2 &screenPos) const {
	if (!m_hasTexture || m_textureWidth == 0 || m_textureHeight == 0) {
		return ImVec2(0, 0);
	}
	float x = (screenPos.x - m_texturePos.x) / m_textureSize.x * m_textureWidth;
	float y =
		(screenPos.y - m_texturePos.y) / m_textureSize.y * m_textureHeight;
	return ImVec2(x, y);
}

} // namespace blot
