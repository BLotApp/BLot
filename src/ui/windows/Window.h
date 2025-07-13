#pragma once

#include <string>
#include <imgui.h>

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

    Window(const std::string& title, Flags flags = Flags::None)
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
    bool isFocused() const { return false; } // TODO: Implement focus tracking
    
    // Window identification
    std::string getName() const { return m_title; }
    std::string getTitle() const { return m_title; }
    
    // Flags
    void setFlags(Flags flags) { m_flags = static_cast<int>(flags); }
    int getFlags() const { return m_flags; }
    
    // Position and size (stubs for now)
    void setPosition(const ImVec2& pos) { /* TODO: Implement */ }
    void setSize(const ImVec2& size) { /* TODO: Implement */ }
    void setMinSize(const ImVec2& size) { /* TODO: Implement */ }
    void setMaxSize(const ImVec2& size) { /* TODO: Implement */ }
    void setAlpha(float alpha) { /* TODO: Implement */ }

    // Rendering - only render() needs to be virtual since each window renders differently
    virtual void render() = 0;  // Pure virtual as it's the main rendering function

protected:
    std::string m_title;
    bool m_isOpen = true;
    int m_flags = 0;
};

// Utility function to combine flags
inline Window::Flags operator|(Window::Flags a, Window::Flags b) {
    return static_cast<Window::Flags>(static_cast<int>(a) | static_cast<int>(b));
}

} // namespace blot 