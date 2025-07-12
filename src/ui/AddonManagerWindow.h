#pragma once

#include "Window.h"
#include <memory>

class AddonManager;

namespace blot {

class AddonManagerWindow : public Window {
public:
    AddonManagerWindow(const std::string& title = "Addon Manager", Flags flags = Flags::None);
    virtual ~AddonManagerWindow() = default;

    void setAddonManager(std::shared_ptr<AddonManager> addonManager);
    virtual void render() override;

    // Window interface overrides
    void show() override;
    void hide() override;
    void close() override;
    void toggle() override;
    bool isOpen() const override;
    bool isVisible() const override;
    bool isFocused() const override;
    bool isHovered() const override;
    bool isDragging() const override;
    bool isResizing() const override;
    void setPosition(const ImVec2&) override;
    void setSize(const ImVec2&) override;
    void setMinSize(const ImVec2&) override;
    void setMaxSize(const ImVec2&) override;
    void setFlags(Window::Flags) override;
    Window::Flags getFlags() const override;
    const std::string& getTitle() const override;
    void setTitle(const std::string&) override;

private:
    std::shared_ptr<AddonManager> m_addonManager;
    void renderAddonManager();
};

} // namespace blot 