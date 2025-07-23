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
#include "core/IAddon.h"

class MyAddon : public blot::IAddon {
public:
    MyAddon() : blot::IAddon("myAddon", "1.0.0") {
        // Optional: Set addon metadata
        setDescription("My custom addon");
        setAuthor("Your Name");
    }
    
    // Override these lifecycle methods for custom behavior
    bool init() override { 
        log("Initializing MyAddon");
        // Your initialization code here
        return true; 
    }
    void setup() override { 
        log("Setting up MyAddon");
        // Your setup code here
        setParameter("myParam", 1.0f);
    }
    void update(float dt) override { 
        // Your update code here
        // Time tracking is handled automatically by IAddon::blotUpdate()
    }
    void draw() override { 
        // Your drawing code here
    }
    void cleanup() override { 
        log("Cleaning up MyAddon");
        // Your cleanup code here
    }
};
```

4. **Register your addon in CMake:**
```cmake
add_library(myAddon STATIC myAddon.cpp)
target_include_directories(myAddon PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
```
Add `add_subdirectory(myAddon)` to `addons/CMakeLists.txt`.

### IAddon Interface Pattern

The `IAddon` interface uses the **Template Method Pattern** (like `IApp`) to ensure proper lifecycle management. This means:

- **Framework controls the flow**: `IAddon::blotUpdate()` always updates time tracking
- **User implements hooks**: Override `update()`, `init()`, etc. for custom behavior
- **Automatic time tracking**: Time is automatically tracked and available via `getTime()`
- **Parameter system**: Built-in parameter management with callbacks via `setParameter()`, `getParameter()`, `onParameterChanged()`

**Important**: The framework calls your methods automatically - no need to call parent methods:
```cpp
void update(float dt) override {
    // Your update code here
    // Time tracking is handled automatically by IAddon::blotUpdate()
}
```

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
