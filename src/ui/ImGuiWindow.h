#pragma once

#include "Window.h"

namespace blot {

class ImGuiWindow : public Window {
public:
    ImGuiWindow(const std::string& title, Flags flags = Flags::None);
    virtual ~ImGuiWindow() = default;

    // Window management
    virtual void show() override;
    virtual void hide() override;
    virtual void close() override;
    virtual void toggle() override;
    
    // State queries
    virtual bool isOpen() const override;
    virtual bool isVisible() const override;
    virtual bool isFocused() const override;
    virtual bool isHovered() const override;
    virtual bool isDragging() const override;
    virtual bool isResizing() const override;

    // Position and size
    virtual void setPosition(const ImVec2& pos) override;
    virtual void setSize(const ImVec2& size) override;
    virtual void setMinSize(const ImVec2& size) override;
    virtual void setMaxSize(const ImVec2& size) override;
    
    virtual ImVec2 getPosition() const override;
    virtual ImVec2 getSize() const override;
    virtual ImVec2 getMinSize() const override;
    virtual ImVec2 getMaxSize() const override;

    // Appearance
    virtual void setAlpha(float alpha) override;
    virtual float getAlpha() const override;
    virtual void setFlags(Flags flags) override;
    virtual int getFlags() const override;

    // Window state
    virtual WindowState& getState() override;
    virtual const WindowState& getState() const override;

    // Window identification
    virtual std::string getName() const override;
    virtual std::string getTitle() const override;

    // Rendering
    virtual void begin() override;
    virtual void end() override;
    virtual void render() override;

    // Event callbacks
    virtual void setOnShow(std::function<void()> callback) override;
    virtual void setOnHide(std::function<void()> callback) override;
    virtual void setOnFocus(std::function<void()> callback) override;
    virtual void setOnBlur(std::function<void()> callback) override;
    virtual void setOnDragStart(std::function<void()> callback) override;
    virtual void setOnDragEnd(std::function<void()> callback) override;
    virtual void setOnResize(std::function<void(const ImVec2&)> callback) override;

protected:
    // Helper methods
    virtual void updateState() override;
    virtual void triggerCallbacks() override;
};

} // namespace blot 