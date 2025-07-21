#pragma once

#include <imgui.h>
#include <string>

namespace blot {

class Window {
  public:
	enum class Flags {
		None = 0,
		NoCollapse = ImGuiWindowFlags_NoCollapse,
		NoResize = ImGuiWindowFlags_NoResize,
		NoMove = ImGuiWindowFlags_NoMove,
		NoDocking = ImGuiWindowFlags_NoDocking,
		NoScrollbar = ImGuiWindowFlags_NoScrollbar,
		NoScrollWithMouse = ImGuiWindowFlags_NoScrollWithMouse,
		NoBringToFrontOnFocus = ImGuiWindowFlags_NoBringToFrontOnFocus,
		NoNavFocus = ImGuiWindowFlags_NoNavFocus,
		AlwaysAutoResize = ImGuiWindowFlags_AlwaysAutoResize,
		NoSavedSettings = ImGuiWindowFlags_NoSavedSettings,
		NoInputs = ImGuiWindowFlags_NoInputs,
		MenuBar = ImGuiWindowFlags_MenuBar,
		HorizontalScrollbar = ImGuiWindowFlags_HorizontalScrollbar,
		NoFocusOnAppearing = ImGuiWindowFlags_NoFocusOnAppearing,
		NoBackground = ImGuiWindowFlags_NoBackground,
		NoDecoration = ImGuiWindowFlags_NoDecoration,
		NoTitleBar = ImGuiWindowFlags_NoTitleBar,
		UnsavedDocument = ImGuiWindowFlags_UnsavedDocument,
		ChildWindow = ImGuiWindowFlags_ChildWindow,
		Tooltip = ImGuiWindowFlags_Tooltip,
		Popup = ImGuiWindowFlags_Popup,
		Modal = ImGuiWindowFlags_Modal,
		ChildMenu = ImGuiWindowFlags_ChildMenu
	};

	Window(const std::string &title, Flags flags = Flags::None)
		: m_title(title), m_flags(static_cast<int>(flags)) {}
	virtual ~Window() = default;

	// Window management
	void show() { m_isOpen = true; }
	void hide() { m_isOpen = false; }
	void close() { m_isOpen = false; }
	void toggle() { m_isOpen = !m_isOpen; }

	// State queries
	bool isOpen() const { return m_isOpen; }
	bool isVisible() const { return m_isOpen; }
	bool isFocused() const { return m_isFocused; }

	// Window identification
	std::string getName() const { return m_title; }
	std::string getTitle() const { return m_title; }

	// Flags
	void setFlags(Flags flags) { m_flags = static_cast<int>(flags); }
	int getFlags() const { return m_flags; }

	// Position and size
	void setPosition(const ImVec2 &pos) { m_position = pos; }
	void setSize(const ImVec2 &size) { m_size = size; }
	void setMinSize(const ImVec2 &size) { m_minSize = size; }
	void setMaxSize(const ImVec2 &size) { m_maxSize = size; }
	void setAlpha(float alpha) { m_alpha = alpha; }

	// Getters for position and size
	ImVec2 getPosition() const { return m_position; }
	ImVec2 getSize() const { return m_size; }
	ImVec2 getMinSize() const { return m_minSize; }
	ImVec2 getMaxSize() const { return m_maxSize; }
	float getAlpha() const { return m_alpha; }

	// Focus management
	void setFocused(bool focused) { m_isFocused = focused; }

	// Helper method for windows to update focus state during rendering
	void updateFocusState() {
		if (ImGui::IsWindowFocused()) {
			m_isFocused = true;
		} else if (ImGui::IsWindowHovered() && !ImGui::IsWindowFocused()) {
			// Keep focus if window is hovered but not focused (transition
			// state)
		} else {
			m_isFocused = false;
		}
	}

	// Non-virtual render: handles ImGui::Begin/End and open/close logic
	// automatically
	void render() {
		if (!m_isOpen)
			return;
		if (ImGui::Begin(m_title.c_str(), &m_isOpen, m_flags)) {
			renderContents();
		}
		ImGui::End();
	}

  protected:
	// Derived classes implement only the window's UI here
	virtual void renderContents() = 0;
	std::string m_title;
	bool m_isOpen = true;
	int m_flags = 0;

	// Window state tracking
	ImVec2 m_position = ImVec2(0, 0);
	ImVec2 m_size = ImVec2(400, 300);
	ImVec2 m_minSize = ImVec2(100, 100);
	ImVec2 m_maxSize = ImVec2(FLT_MAX, FLT_MAX);
	bool m_isFocused = false;
	float m_alpha = 1.0f;
};

// Utility function to combine flags
inline Window::Flags operator|(Window::Flags a, Window::Flags b) {
	return static_cast<Window::Flags>(static_cast<int>(a) |
									  static_cast<int>(b));
}

} // namespace blot
