#pragma once

#include <string>
#include <functional>
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
        UnsavedDocument = ImGuiWindowFlags_UnsavedDocument,
        ChildWindow = ImGuiWindowFlags_ChildWindow,
        Tooltip = ImGuiWindowFlags_Tooltip,
        Popup = ImGuiWindowFlags_Popup,
        Modal = ImGuiWindowFlags_Modal,
        ChildMenu = ImGuiWindowFlags_ChildMenu
    };

    struct WindowState {
        bool isOpen = true;
        bool isVisible = true;
        bool isFocused = false;
        bool isHovered = false;
        bool isDragging = false;
        bool isResizing = false;
        ImVec2 position = ImVec2(0, 0);
        ImVec2 size = ImVec2(400, 300);
        ImVec2 minSize = ImVec2(100, 100);
        ImVec2 maxSize = ImVec2(FLT_MAX, FLT_MAX);
        float alpha = 1.0f;
        int flags = 0;
    };

    Window(const std::string& title, Flags flags = Flags::None)
        : m_title(title) { m_state.flags = static_cast<int>(flags); }
    virtual ~Window() = default;

    // Window management
    virtual void show() = 0;
    virtual void hide() = 0;
    virtual void close() = 0;
    virtual void toggle() = 0;
    
    // State queries
    virtual bool isOpen() const = 0;
    virtual bool isVisible() const = 0;
    virtual bool isFocused() const = 0;
    virtual bool isHovered() const = 0;
    virtual bool isDragging() const = 0;
    virtual bool isResizing() const = 0;

    // Position and size
    virtual void setPosition(const ImVec2& pos) = 0;
    virtual void setSize(const ImVec2& size) = 0;
    virtual void setMinSize(const ImVec2& size) = 0;
    virtual void setMaxSize(const ImVec2& size) = 0;
    
    virtual ImVec2 getPosition() const = 0;
    virtual ImVec2 getSize() const = 0;
    virtual ImVec2 getMinSize() const = 0;
    virtual ImVec2 getMaxSize() const = 0;

    // Appearance
    virtual void setAlpha(float alpha) = 0;
    virtual float getAlpha() const = 0;
    virtual void setFlags(Flags flags) = 0;
    virtual int getFlags() const = 0;

    // Window state
    virtual WindowState& getState() = 0;
    virtual const WindowState& getState() const = 0;

    // Rendering
    virtual void begin() = 0;
    virtual void end() = 0;
    virtual void render() = 0;

    // Event callbacks
    virtual void setOnShow(std::function<void()> callback) = 0;
    virtual void setOnHide(std::function<void()> callback) = 0;
    virtual void setOnFocus(std::function<void()> callback) = 0;
    virtual void setOnBlur(std::function<void()> callback) = 0;
    virtual void setOnDragStart(std::function<void()> callback) = 0;
    virtual void setOnDragEnd(std::function<void()> callback) = 0;
    virtual void setOnResize(std::function<void(const ImVec2&)> callback) = 0;

protected:
    std::string m_title;
    WindowState m_state;
    bool m_wasOpen = false;
    bool m_wasVisible = false;
    bool m_wasFocused = false;
    bool m_wasHovered = false;
    bool m_wasDragging = false;
    bool m_wasResizing = false;

    // Callbacks
    std::function<void()> m_onShow;
    std::function<void()> m_onHide;
    std::function<void()> m_onFocus;
    std::function<void()> m_onBlur;
    std::function<void()> m_onDragStart;
    std::function<void()> m_onDragEnd;
    std::function<void(const ImVec2&)> m_onResize;

    // Helper methods
    virtual void updateState() = 0;
    virtual void triggerCallbacks() = 0;
};

// Utility function to combine flags
inline Window::Flags operator|(Window::Flags a, Window::Flags b) {
    return static_cast<Window::Flags>(static_cast<int>(a) | static_cast<int>(b));
}

} // namespace blot 