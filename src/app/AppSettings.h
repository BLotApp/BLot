#pragma once

#include <string>
#include <vector>
#include <imgui.h>

struct AppSettings {
    // Window visibility flags (some now managed by WindowManager)
    bool showDemoWindow = false;
    bool showFileBrowser = false;
    
    // Demo windows
    bool showImPlotDemo = false;
    bool showImGuiMarkdownDemo = false;
    bool showMarkdownEditor = false;
    bool showMarkdownViewer = false;
    
    // Info panel display

    int mouseCoordinateSystem = 0; // 0=Screen, 1=App, 2=Window
    static const char* coordinateSystemNames[];
    
    // Color and style settings
    ImVec4 fillColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    ImVec4 strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
    float strokeWidth = 2.0f;
    int strokeCap = 0; // 0=Butt, 1=Square, 2=Round
    int strokeJoin = 0; // 0=Miter, 1=Bevel, 2=Round
    float dashPattern[4] = {0};
    int dashCount = 0;
    float dashOffset = 0.0f;
    
    // Shape settings
    int selectedShape = 0; // 0=Rectangle, 1=Ellipse, 2=Line, 3=Polygon, 4=Star
    int polygonSides = 5;
    
    // Swatches
    std::vector<ImVec4> swatches = {
        ImVec4(0,0,0,1), ImVec4(1,1,1,1), ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(0,0,1,1)
    };
    int activeSwatchType = 0; // 0 = fill, 1 = stroke
    std::string swatchesFile = "swatches.json";
    bool showSwatches = false;
    bool showStrokePalette = false;
    
    // Markdown
    std::string markdownEditorBuffer;
    std::string loadedMarkdownPath;
    std::string loadedMarkdown;
    
    // Theme
    std::string lastThemePath = "theme.json";
    
    // Pen tool state
    std::vector<ImVec2> penPoints;
    std::vector<ImVec2> penHandles;
    bool penDraggingHandle = false;
    int penHandleIndex = -1;
    
    // Tool state
    ImVec2 toolStartPos = ImVec2(0,0);
    bool toolActive = false;
    
    // Settings file
    std::string settingsFile = "appsettings.json";
    
    // Methods
    void saveSettings();
    void loadSettings();
    void resetToDefaults();
    bool showMenuTips = true;
}; 