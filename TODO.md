# TODO

- [ ] Learn more about Continuous Integration (CI) for C++ projects
    - Research how to set up CI pipelines (e.g., GitHub Actions)
    - Explore tools for code quality, formatting (clang-format), and linting (clang-tidy)
    - Try adding a basic CI workflow to this project to automate builds and checks

## UI Improvements

### Multi-Column Support
- [ ] Add multi-column support to ThemePanel
- [ ] Extend multi-column support to other window types
- [ ] Implement responsive column layouts
- [ ] Add column configuration options

## Features

### Generalized Properties System
- [ ] Implement generalized properties system with type-safe property handling
    - [x] Define property type enum and struct (see propTypes, blotProp)
    - [x] Document and enforce the GetProperties() convention for ECS components (see docs/components.md)
    - [ ] Add support for dynamic property types (bool, int, float, string, ImVec4, etc.)
    - [ ] Create generic property editor UI that can handle any property type
    - [ ] Integrate with ECS system for automatic property reflection (detects GetProperties())
    - [ ] Add property change callbacks and undo/redo support
    - [ ] Implement property serialization/deserialization (save/load)
    - [ ] Add unit tests for property system
    - [ ] Document usage, extension, and advanced type safety (e.g., std::variant for sProp::data)
    - [ ] Warn in logs if a component does not implement GetProperties() (already implemented)

## App State Save/Load & Settings Serialization
- [ ] Add getSettings/setSettings methods to each manager (Mui, ECS Manager, MCanvas, etc.) for modular settings serialization/deserialization
- [ ] Implement to_json/from_json helpers for all custom types used in settings (e.g., glm::vec2, glm::vec3, ImVec4, etc.)
- [ ] Refactor app-level save/load logic to gather settings from all managers using getSettings, and restore them using setSettings
- [ ] Finish implementing save/load settings for all manager and configurable classes (see ISettings interface). Ensure all relevant variables are serialized.
- [ ] Save document format JSON. Consider saving settings in the output metadata for project files.

## Bug Fixes

## Performance

## Logging System
- [x] Replace all std::cout, std::cerr, and printf logging in src/ with spdlog
- [x] Remove all unnecessary #include <iostream> from headers and sources
- [x] Fix all spdlog pointer formatting (use reinterpret_cast<uintptr_t>(ptr) and 0x{:X})
- [x] Fix all OpenGL handle logging (cast to unsigned int)
- [x] Build and verify all logging changes
- [ ] Consider adding a custom blot::info/blot::error logging wrapper for pointer/handle safety and context
- [ ] Consider implementing a custom fmt::formatter for pointers to automate safe pointer logging with spdlog

## Test Engine 
- [ ] Integrate ImGui Test Engine (https://github.com/ocornut/imgui_test_engine) for automated UI testing
- [ ] Add/improve unit tests for logging and error handling 
- [ ] Refactor NodeEditor into an addon (move NodeEditorWindow and related code to addons/bxNodeEditor, update CMake, and register as an addon) 

## Managers Management
- [ ] Consider moving all manager classes to `core/managers/` (with or without subfolders) for better discoverability and architectural clarity. This would involve:
    - Moving all manager headers and sources from their current locations (e.g., `ui/`, `rendering/`, `ecs/`, etc.) to `core/managers/`.
    - Optionally organizing by domain within `core/managers/` (e.g., `core/managers/ui/`, `core/managers/rendering/`).
    - Updating all includes throughout the codebase.
    - Ensuring no manager headers/sources remain in other folders.
    - Reviewing for any domain encapsulation concerns. 

- [ ] Consider moving UI subsystem (ImGui/Mui) into an optional addon for lighter core builds 
