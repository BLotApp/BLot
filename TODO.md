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
    - [x] Document and enforce the GetProperties() convention for ECS components (see DOCS/components.md)
    - [ ] Add support for dynamic property types (bool, int, float, string, ImVec4, etc.)
    - [ ] Create generic property editor UI that can handle any property type
    - [ ] Integrate with ECS system for automatic property reflection (detects GetProperties())
    - [ ] Add property change callbacks and undo/redo support
    - [ ] Implement property serialization/deserialization (save/load)
    - [ ] Add unit tests for property system
    - [ ] Document usage, extension, and advanced type safety (e.g., std::variant for sProp::data)
    - [ ] Warn in logs if a component does not implement GetProperties() (already implemented)

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

## Documentation 
- [ ] Integrate ImGui Test Engine (https://github.com/ocornut/imgui_test_engine) for automated UI testing
- [ ] Add/improve unit tests for logging and error handling 