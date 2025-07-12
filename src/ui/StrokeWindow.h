#pragma once

#include "Window.h"
#include <imgui.h>
#include <blend2d.h>
#include <functional>
#include <vector>
#include <string>

class StrokeWindow : public blot::Window {
public:
    // Callback types for stroke changes
    using StrokeWidthCallback = std::function<void(double)>;
    using StrokeCapCallback = std::function<void(BLStrokeCap)>;
    using StrokeJoinCallback = std::function<void(BLStrokeJoin)>;
    using StrokeMiterLimitCallback = std::function<void(double)>;
    using StrokeDashArrayCallback = std::function<void(const std::vector<double>&)>;
    using StrokeDashOffsetCallback = std::function<void(double)>;
    using StrokeTransformOrderCallback = std::function<void(BLStrokeTransformOrder)>;

    StrokeWindow(const std::string& title = "Stroke Window", blot::Window::Flags flags = blot::Window::Flags::None);
    ~StrokeWindow() = default;

    // Main render function
    void render() override;

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
    void setFlags(blot::Window::Flags) override;
    int getFlags() const override;
    std::string getTitle() const override;

    // Getters for current stroke settings
    double getStrokeWidth() const { return m_strokeWidth; }
    BLStrokeCap getStrokeCap() const { return m_strokeCap; }
    BLStrokeJoin getStrokeJoin() const { return m_strokeJoin; }
    double getMiterLimit() const { return m_miterLimit; }
    const std::vector<double>& getDashArray() const { return m_dashArray; }
    double getDashOffset() const { return m_dashOffset; }
    BLStrokeTransformOrder getTransformOrder() const { return m_transformOrder; }

    // Setters for stroke settings
    void setStrokeWidth(double width);
    void setStrokeCap(BLStrokeCap cap);
    void setStrokeJoin(BLStrokeJoin join);
    void setMiterLimit(double limit);
    void setDashArray(const std::vector<double>& dashArray);
    void setDashOffset(double offset);
    void setTransformOrder(BLStrokeTransformOrder order);

    // Callback setters
    void setStrokeWidthCallback(StrokeWidthCallback callback) { m_strokeWidthCallback = callback; }
    void setStrokeCapCallback(StrokeCapCallback callback) { m_strokeCapCallback = callback; }
    void setStrokeJoinCallback(StrokeJoinCallback callback) { m_strokeJoinCallback = callback; }
    void setMiterLimitCallback(StrokeMiterLimitCallback callback) { m_miterLimitCallback = callback; }
    void setDashArrayCallback(StrokeDashArrayCallback callback) { m_dashArrayCallback = callback; }
    void setDashOffsetCallback(StrokeDashOffsetCallback callback) { m_dashOffsetCallback = callback; }
    void setTransformOrderCallback(StrokeTransformOrderCallback callback) { m_transformOrderCallback = callback; }

    // Utility functions
    void resetToDefaults();
    BLStrokeOptions getBlend2DStrokeOptions() const;

private:
    // Stroke properties
    double m_strokeWidth = 1.0;
    BLStrokeCap m_strokeCap = BL_STROKE_CAP_BUTT;
    BLStrokeJoin m_strokeJoin = BL_STROKE_JOIN_MITER_CLIP;
    double m_miterLimit = 4.0;
    std::vector<double> m_dashArray;
    double m_dashOffset = 0.0;
    BLStrokeTransformOrder m_transformOrder = BL_STROKE_TRANSFORM_ORDER_AFTER;

    // Callbacks
    StrokeWidthCallback m_strokeWidthCallback;
    StrokeCapCallback m_strokeCapCallback;
    StrokeJoinCallback m_strokeJoinCallback;
    StrokeMiterLimitCallback m_miterLimitCallback;
    StrokeDashArrayCallback m_dashArrayCallback;
    StrokeDashOffsetCallback m_dashOffsetCallback;
    StrokeTransformOrderCallback m_transformOrderCallback;

    // UI state
    bool m_showAdvanced = false;
    std::string m_dashArrayInput = "";
    bool m_dashArrayInputValid = true;

    // Helper functions
    void renderBasicSettings();
    void renderAdvancedSettings();
    void renderDashArrayEditor();
    void updateDashArrayFromInput();
    void updateDashArrayInput();
    
    // String conversion helpers
    std::string strokeCapToString(BLStrokeCap cap) const;
    std::string strokeJoinToString(BLStrokeJoin join) const;
    std::string transformOrderToString(BLStrokeTransformOrder order) const;
    
    // Preset dash patterns
    void applyDashPreset(const std::string& presetName);
    void renderDashPresets();
}; 