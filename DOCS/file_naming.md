# File Naming Conventions

This document describes the file naming conventions used throughout the project to ensure consistency and clarity.

---

## General Rules
- **Use lowercase letters and underscores** for all filenames (e.g., `main_menu_bar.cpp`, `canvas_manager.h`, `folder_structure.md`).
- **Avoid spaces and special characters** in filenames.
- **Use descriptive names** that reflect the file's purpose or contents.

---

## Source Code Files
- C++ source and header files: `snake_case` (e.g., `canvas_manager.cpp`, `ecs_manager.h`).
- Class names inside files may use `CamelCase` (e.g., `class CanvasManager` in `canvas_manager.cpp`).
- Group related files by subsystem or feature (e.g., `src/ui/`, `src/ecs/`).

---

## Documentation Files
- All documentation files in the `DOCS/` folder should use lowercase and underscores (e.g., `file_naming.md`, `folder_structure.md`).
- Use the `.md` extension for Markdown files.

---

## Exceptions
- Third-party libraries and dependencies may use their own naming conventions. Do not rename these files.

---

## Rationale
- Consistent naming makes the codebase easier to navigate and maintain.
- Lowercase and underscores are cross-platform friendly and avoid issues on case-insensitive filesystems.

---

For questions or suggestions, please contact a maintainer. 