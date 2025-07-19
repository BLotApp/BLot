# Creating Addons for Blot

This guide explains how to create a new addon for the Blot framework.

## What is an Addon?
An addon is a self-contained module that plugs into the framework. Addons can:
- Register new renderers, input handlers, or UI components
- Add new commands, scripting, or integrations
- Be enabled/disabled at runtime or build time

## Addon Structure
Each addon lives in its own folder under `addons/` and typically contains:
- Source code (e.g., `MyAddon.cpp`, `MyAddon.h`)
- `addon.json` metadata file
- `CMakeLists.txt` for building the addon

## Creating a New Addon
1. **Create a new folder in `addons/`** (e.g., `addons/myAddon`)
2. **Add your source files** (e.g., `myAddon.cpp`, `myAddon.h`)
3. **Inherit from `AddonBase`** and implement the required lifecycle methods:

```cpp
#include "framework/AddonBase.h"
class MyAddon : public AddonBase {
public:
    MyAddon() : AddonBase("myAddon", "1.0.0") {}
    bool init() override { /* ... */ return true; }
    void setup() override {}
    void update(float dt) override {}
    void draw() override {}
    void cleanup() override {}
};
```

4. **Register your addon in CMake:**
```cmake
add_library(myAddon STATIC myAddon.cpp)
target_include_directories(myAddon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```
Add `add_subdirectory(myAddon)` to `addons/CMakeLists.txt`.

5. **Add an `addon.json` file** with metadata:
```json
{
  "name": "myAddon",
  "version": "1.0.0",
  "dependencies": []
}
```

6. **Register features (optional):**
If your addon provides a renderer, input system, etc., register it with the appropriate manager or registry (see `bxBlend2DAddon` for an example). For renderers, use the `RendererRegistry` to register your factory function.

## Example: Renderer Addon
See `addons/bxBlend2D/` for a full example of a renderer addon that registers a new renderer with the framework via the registry.

## CMake and Include Paths
- All addons should use umbrella headers (e.g., `#include "core/U_core.h"`, `#include "rendering/U_rendering.h"`) for framework access.
- The top-level `CMakeLists.txt` adds all major `src` subdirectories to the include path, so you can use project-root-relative includes everywhere.
- Avoid direct includes of other addons; use the registry or manager interfaces for integration.

## Best Practices
- Use unique names for your addon and its classes
- Keep addons self-contained
- Use the dependency system if your addon relies on others
- Document your addon in its own README

## Loading and Enabling Addons
Addons are discovered and loaded by the `MAddon`. You can enable/disable addons via config or at runtime. 
