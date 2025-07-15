# AppPaths Utility

## Purpose

The `AppPaths` utility centralizes all resource, data, and configuration paths for the application. This avoids hardcoding paths throughout the codebase, making it easier to change resource locations, support different platforms, and maintain clean separation between assets, user data, and config.

## Usage

Include the header:
```cpp
#include "AppPaths.h"
```

Use the provided functions to get paths:
```cpp
std::string assets = AppPaths::getAssetsDir();
std::string fonts = AppPaths::getFontsDir();
std::string workspaces = AppPaths::getWorkspacesDir();
std::string imguiIni = AppPaths::getImGuiIniPath();
```

## Best Practices
- **Never hardcode resource/data/config paths in your code.**
- Use the `AppPaths` utility everywhere you need a path to assets, user data, or config.
- If you add new resource types, add a function to `AppPaths`.
- For cross-platform support, update `AppPaths` to use platform-specific conventions if needed.

## Example Implementation
See `src/AppPaths.h` and `src/AppPaths.cpp` for the current implementation.

## Extending
- To support user overrides, environment variables, or platform-specific locations, update the functions in `AppPaths` accordingly.
- For more advanced needs, consider making `AppPaths` a singleton with runtime configuration. 