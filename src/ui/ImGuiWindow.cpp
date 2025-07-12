#include "ImGuiWindow.h"
#include <imgui.h>

namespace blot {

ImGuiWindow::ImGuiWindow(const std::string& title, Flags flags)
    : Window(title, flags) {
}

void ImGuiWindow::show() {
    m_state.isVisible = true;
    m_state.isOpen = true;
}

void ImGuiWindow::hide() {
    m_state.isVisible = false;
}

void ImGuiWindow::close() {
    m_state.isOpen = false;
}

void ImGuiWindow::toggle() {
    m_state.isOpen = !m_state.isOpen;
}

bool ImGuiWindow::isOpen() const {
    return m_state.isOpen;
}

bool ImGuiWindow::isVisible() const {
    return m_state.isVisible;
}

bool ImGuiWindow::isFocused() const {
    return m_state.isFocused;
}

bool ImGuiWindow::isHovered() const {
    return m_state.isHovered;
}

bool ImGuiWindow::isDragging() const {
    return m_state.isDragging;
}

bool ImGuiWindow::isResizing() const {
    return m_state.isResizing;
}

void ImGuiWindow::setPosition(const ImVec2& pos) {
    m_state.position = pos;
}

void ImGuiWindow::setSize(const ImVec2& size) {
    m_state.size = size;
}

void ImGuiWindow::setMinSize(const ImVec2& size) {
    m_state.minSize = size;
}

void ImGuiWindow::setMaxSize(const ImVec2& size) {
    m_state.maxSize = size;
}

ImVec2 ImGuiWindow::getPosition() const {
    return m_state.position;
}

ImVec2 ImGuiWindow::getSize() const {
    return m_state.size;
}

ImVec2 ImGuiWindow::getMinSize() const {
    return m_state.minSize;
}

ImVec2 ImGuiWindow::getMaxSize() const {
    return m_state.maxSize;
}

void ImGuiWindow::setAlpha(float alpha) {
    m_state.alpha = alpha;
}

float ImGuiWindow::getAlpha() const {
    return m_state.alpha;
}

void ImGuiWindow::setFlags(Flags flags) {
    m_state.flags = static_cast<int>(flags);
}

int ImGuiWindow::getFlags() const {
    return m_state.flags;
}

Window::WindowState& ImGuiWindow::getState() {
    return m_state;
}

const Window::WindowState& ImGuiWindow::getState() const {
    return m_state;
}

std::string ImGuiWindow::getName() const {
    return m_title;
}

std::string ImGuiWindow::getTitle() const {
    return m_title;
}

void ImGuiWindow::begin() {
    // Set window properties
    ImGui::SetNextWindowSize(m_state.size, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowPos(m_state.position, ImGuiCond_FirstUseEver);
    
    // Begin the window
    m_state.isOpen = ImGui::Begin(m_title.c_str(), &m_state.isOpen, m_state.flags);
    
    if (m_state.isOpen) {
        updateState();
    }
}

void ImGuiWindow::end() {
    if (m_state.isOpen) {
        ImGui::End();
    }
}

void ImGuiWindow::render() {
    begin();
    if (m_state.isOpen) {
        // Default rendering - subclasses should override this
        ImGui::Text("ImGuiWindow base class - override render() in derived class");
        end();
    }
}

void ImGuiWindow::setOnShow(std::function<void()> callback) {
    m_onShow = callback;
}

void ImGuiWindow::setOnHide(std::function<void()> callback) {
    m_onHide = callback;
}

void ImGuiWindow::setOnFocus(std::function<void()> callback) {
    m_onFocus = callback;
}

void ImGuiWindow::setOnBlur(std::function<void()> callback) {
    m_onBlur = callback;
}

void ImGuiWindow::setOnDragStart(std::function<void()> callback) {
    m_onDragStart = callback;
}

void ImGuiWindow::setOnDragEnd(std::function<void()> callback) {
    m_onDragEnd = callback;
}

void ImGuiWindow::setOnResize(std::function<void(const ImVec2&)> callback) {
    m_onResize = callback;
}

void ImGuiWindow::updateState() {
    // Update window state from ImGui
    m_state.isFocused = ImGui::IsWindowFocused();
    m_state.isHovered = ImGui::IsWindowHovered();
    m_state.isDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    m_state.isResizing = false; // ImGui doesn't provide direct resize detection
    
    // Update position and size
    ImVec2 pos = ImGui::GetWindowPos();
    ImVec2 size = ImGui::GetWindowSize();
    
    if (pos.x != m_state.position.x || pos.y != m_state.position.y) {
        m_state.position = pos;
        if (m_onResize) {
            m_onResize(size);
        }
    }
    
    if (size.x != m_state.size.x || size.y != m_state.size.y) {
        m_state.size = size;
        if (m_onResize) {
            m_onResize(size);
        }
    }
    
    triggerCallbacks();
}

void ImGuiWindow::triggerCallbacks() {
    // Check for state changes and trigger callbacks
    if (!m_wasOpen && m_state.isOpen && m_onShow) {
        m_onShow();
    }
    if (m_wasOpen && !m_state.isOpen && m_onHide) {
        m_onHide();
    }
    if (!m_wasFocused && m_state.isFocused && m_onFocus) {
        m_onFocus();
    }
    if (m_wasFocused && !m_state.isFocused && m_onBlur) {
        m_onBlur();
    }
    if (!m_wasDragging && m_state.isDragging && m_onDragStart) {
        m_onDragStart();
    }
    if (m_wasDragging && !m_state.isDragging && m_onDragEnd) {
        m_onDragEnd();
    }
    
    // Update previous state
    m_wasOpen = m_state.isOpen;
    m_wasVisible = m_state.isVisible;
    m_wasFocused = m_state.isFocused;
    m_wasHovered = m_state.isHovered;
    m_wasDragging = m_state.isDragging;
    m_wasResizing = m_state.isResizing;
}

} // namespace blot 