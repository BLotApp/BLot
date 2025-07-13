#pragma once

#include "Window.h"
#include <functional>

namespace blot {

class ImGuiWindow : public Window {
public:
    ImGuiWindow(const std::string& title, Flags flags = Flags::None);
    virtual ~ImGuiWindow() = default;

    // Rendering
    virtual void render() override;

    // Custom render callback
    void setRenderCallback(std::function<void()> callback);

protected:
    // Custom render callback
    std::function<void()> m_renderCallback;
};

} // namespace blot 