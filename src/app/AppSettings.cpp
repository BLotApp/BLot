#include "AppSettings.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

const char* AppSettings::coordinateSystemNames[] = { "Screen", "App", "Window" };

void AppSettings::saveSettings() {
    nlohmann::json j;
    
    // Window visibility
    j["windows"]["showCodeEditor"] = showCodeEditor;
    j["windows"]["showCanvas"] = showCanvas;
    j["windows"]["showProperties"] = showProperties;
    j["windows"]["showAddonManager"] = showAddonManager;
    j["windows"]["showNodeEditor"] = showNodeEditor;
    j["windows"]["showToolbar"] = showToolbar;
    j["windows"]["showDemoWindow"] = showDemoWindow;
    j["windows"]["showFileBrowser"] = showFileBrowser;
    j["windows"]["showThemeEditor"] = showThemeEditor;
    j["windows"]["showImPlotDemo"] = showImPlotDemo;
    j["windows"]["showImGuiMarkdownDemo"] = showImGuiMarkdownDemo;
    j["windows"]["showMarkdownEditor"] = showMarkdownEditor;
    j["windows"]["showMarkdownViewer"] = showMarkdownViewer;
    j["windows"]["showMouseCoordinates"] = showMouseCoordinates;
    j["windows"]["showSwatches"] = showSwatches;
    j["windows"]["showStrokePalette"] = showStrokePalette;
    
    // Mouse coordinates
    j["mouse"]["coordinateSystem"] = mouseCoordinateSystem;
    
    // Colors
    j["colors"]["fillColor"] = {fillColor.x, fillColor.y, fillColor.z, fillColor.w};
    j["colors"]["strokeColor"] = {strokeColor.x, strokeColor.y, strokeColor.z, strokeColor.w};
    j["colors"]["strokeWidth"] = strokeWidth;
    j["colors"]["strokeCap"] = strokeCap;
    j["colors"]["strokeJoin"] = strokeJoin;
    j["colors"]["dashPattern"] = {dashPattern[0], dashPattern[1], dashPattern[2], dashPattern[3]};
    j["colors"]["dashCount"] = dashCount;
    j["colors"]["dashOffset"] = dashOffset;
    j["colors"]["activeSwatchType"] = activeSwatchType;
    
    // Shapes
    j["shapes"]["selectedShape"] = selectedShape;
    j["shapes"]["polygonSides"] = polygonSides;
    
    // Swatches
    j["swatches"]["file"] = swatchesFile;
    j["swatches"]["colors"] = nlohmann::json::array();
    for (const auto& color : swatches) {
        j["swatches"]["colors"].push_back({color.x, color.y, color.z, color.w});
    }
    
    // Markdown
    j["markdown"]["buffer"] = markdownEditorBuffer;
    j["markdown"]["loadedPath"] = loadedMarkdownPath;
    j["markdown"]["loadedContent"] = loadedMarkdown;
    
    // Theme
    j["theme"]["lastPath"] = lastThemePath;
    
    // Files
    j["files"]["settingsFile"] = settingsFile;
    
    std::ofstream out(settingsFile);
    if (out.is_open()) {
        out << j.dump(2);
        std::cout << "[Settings] Saved settings to " << settingsFile << std::endl;
    } else {
        std::cerr << "[Settings] Failed to save settings to " << settingsFile << std::endl;
    }
}

void AppSettings::loadSettings() {
    std::ifstream in(settingsFile);
    if (!in.is_open()) {
        std::cout << "[Settings] No settings file found, using defaults" << std::endl;
        return;
    }
    
    try {
        nlohmann::json j;
        in >> j;
        
        // Window visibility
        if (j.contains("windows")) {
            auto& windows = j["windows"];
            if (windows.contains("showCodeEditor")) showCodeEditor = windows["showCodeEditor"];
            if (windows.contains("showCanvas")) showCanvas = windows["showCanvas"];
            if (windows.contains("showProperties")) showProperties = windows["showProperties"];
            if (windows.contains("showAddonManager")) showAddonManager = windows["showAddonManager"];
            if (windows.contains("showNodeEditor")) showNodeEditor = windows["showNodeEditor"];
            if (windows.contains("showToolbar")) showToolbar = windows["showToolbar"];
            if (windows.contains("showDemoWindow")) showDemoWindow = windows["showDemoWindow"];
            if (windows.contains("showFileBrowser")) showFileBrowser = windows["showFileBrowser"];
            if (windows.contains("showThemeEditor")) showThemeEditor = windows["showThemeEditor"];
            if (windows.contains("showImPlotDemo")) showImPlotDemo = windows["showImPlotDemo"];
            if (windows.contains("showImGuiMarkdownDemo")) showImGuiMarkdownDemo = windows["showImGuiMarkdownDemo"];
            if (windows.contains("showMarkdownEditor")) showMarkdownEditor = windows["showMarkdownEditor"];
            if (windows.contains("showMarkdownViewer")) showMarkdownViewer = windows["showMarkdownViewer"];
            if (windows.contains("showMouseCoordinates")) showMouseCoordinates = windows["showMouseCoordinates"];
            if (windows.contains("showSwatches")) showSwatches = windows["showSwatches"];
            if (windows.contains("showStrokePalette")) showStrokePalette = windows["showStrokePalette"];
        }
        
        // Mouse coordinates
        if (j.contains("mouse") && j["mouse"].contains("coordinateSystem")) {
            mouseCoordinateSystem = j["mouse"]["coordinateSystem"];
        }
        
        // Colors
        if (j.contains("colors")) {
            auto& colors = j["colors"];
            if (colors.contains("fillColor")) {
                auto& fc = colors["fillColor"];
                fillColor = ImVec4(fc[0], fc[1], fc[2], fc[3]);
            }
            if (colors.contains("strokeColor")) {
                auto& sc = colors["strokeColor"];
                strokeColor = ImVec4(sc[0], sc[1], sc[2], sc[3]);
            }
            if (colors.contains("strokeWidth")) strokeWidth = colors["strokeWidth"];
            if (colors.contains("strokeCap")) strokeCap = colors["strokeCap"];
            if (colors.contains("strokeJoin")) strokeJoin = colors["strokeJoin"];
            if (colors.contains("dashPattern")) {
                auto& dp = colors["dashPattern"];
                for (int i = 0; i < 4; i++) dashPattern[i] = dp[i];
            }
            if (colors.contains("dashCount")) dashCount = colors["dashCount"];
            if (colors.contains("dashOffset")) dashOffset = colors["dashOffset"];
            if (colors.contains("activeSwatchType")) activeSwatchType = colors["activeSwatchType"];
        }
        
        // Shapes
        if (j.contains("shapes")) {
            auto& shapes = j["shapes"];
            if (shapes.contains("selectedShape")) selectedShape = shapes["selectedShape"];
            if (shapes.contains("polygonSides")) polygonSides = shapes["polygonSides"];
        }
        
        // Swatches
        if (j.contains("swatches")) {
            auto& swatchesJson = j["swatches"];
            if (swatchesJson.contains("file")) swatchesFile = swatchesJson["file"];
            if (swatchesJson.contains("colors")) {
                swatches.clear();
                for (const auto& color : swatchesJson["colors"]) {
                    swatches.push_back(ImVec4(color[0], color[1], color[2], color[3]));
                }
            }
        }
        
        // Markdown
        if (j.contains("markdown")) {
            auto& markdown = j["markdown"];
            if (markdown.contains("buffer")) markdownEditorBuffer = markdown["buffer"];
            if (markdown.contains("loadedPath")) loadedMarkdownPath = markdown["loadedPath"];
            if (markdown.contains("loadedContent")) loadedMarkdown = markdown["loadedContent"];
        }
        
        // Theme
        if (j.contains("theme") && j["theme"].contains("lastPath")) {
            lastThemePath = j["theme"]["lastPath"];
        }
        
        // Files
        if (j.contains("files") && j["files"].contains("settingsFile")) {
            settingsFile = j["files"]["settingsFile"];
        }
        
        std::cout << "[Settings] Loaded settings from " << settingsFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "[Settings] Error loading settings: " << e.what() << std::endl;
    }
}

void AppSettings::resetToDefaults() {
    // Reset all settings to default values
    showCodeEditor = true;
    showCanvas = true;
    showProperties = true;
    showAddonManager = false;
    showNodeEditor = false;
    showToolbar = true;
    showDemoWindow = false;
    showFileBrowser = false;
    showThemeEditor = false;
    showImPlotDemo = false;
    showImGuiMarkdownDemo = false;
    showMarkdownEditor = false;
    showMarkdownViewer = false;
    showMouseCoordinates = true;
    mouseCoordinateSystem = 0;
    
    // Set default colors to something visible
    fillColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);  // Red fill
    strokeColor = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // Black stroke
    strokeWidth = 2.0f;
    strokeCap = 0;
    strokeJoin = 0;
    for (int i = 0; i < 4; i++) dashPattern[i] = 0;
    dashCount = 0;
    dashOffset = 0.0f;
    activeSwatchType = 0;
    
    selectedShape = 0;
    polygonSides = 5;
    
    swatches = {
        ImVec4(0,0,0,1), ImVec4(1,1,1,1), ImVec4(1,0,0,1), ImVec4(0,1,0,1), ImVec4(0,0,1,1)
    };
    swatchesFile = "swatches.json";
    showSwatches = false;
    showStrokePalette = false;
    
    markdownEditorBuffer.clear();
    loadedMarkdownPath.clear();
    loadedMarkdown.clear();
    
    lastThemePath = "theme.json";
    
    penPoints.clear();
    penHandles.clear();
    penDraggingHandle = false;
    penHandleIndex = -1;
    
    toolStartPos = ImVec2(0,0);
    toolActive = false;
    
    std::cout << "[Settings] Reset to defaults" << std::endl;
} 