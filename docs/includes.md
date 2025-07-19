# Includes and Umbrella Headers

## Umbrella Headers

To simplify includes and improve modularity, the codebase uses umbrella headers for all major modules:

- `core/U_core.h` — includes all major core headers (MAddon, BlotEngine, IApp, etc.)
- `ui/U_ui.h` — includes all major UI headers (UI Manager, ImGuiRenderer, TextRenderer, etc.)
- `rendering/U_rendering.h` — includes all major rendering headers (Renderer, MRendering, RendererRegistry, etc.)
- `ecs/ecs.h` — includes all ECS types and managers

**Usage:**
Instead of including multiple headers from a module, just include the umbrella header:

```cpp
#include "core/U_core.h"
#include "ui/U_ui.h"
#include "rendering/U_rendering.h"
#include "ecs/ecs.h"
```

This makes all public types from that module available everywhere with a single include.

## CMake Include Paths

The top-level `CMakeLists.txt` adds all major `src` subdirectories to the include path for all targets:

```cmake
target_include_directories(blot PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/ui
    ${CMAKE_SOURCE_DIR}/src/rendering
    ${CMAKE_SOURCE_DIR}/src/ecs
    ${CMAKE_SOURCE_DIR}/src/core/canvas
    # ...add others as needed
)
```

This allows you to always use project-root-relative includes (e.g., `#include "core/MAddon.h"`) from anywhere in the codebase.

## Best Practices
- Add new public headers to the appropriate umbrella header.
- New modules should provide their own umbrella header for consistency.
- Use forward declarations in headers when possible; include umbrella headers in `.cpp` files for implementation.
- For third-party or local includes, continue to use their standard paths. 
