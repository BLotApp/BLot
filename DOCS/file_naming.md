# File Naming Conventions

This document describes the file naming conventions used throughout the project to ensure consistency and clarity.

---

## General Rules
- **Use CamelCase for C++ source and header files** (e.g., `WindowManager.h`, `CanvasManager.cpp`), matching the class name.
- **Use lowercase letters and underscores** for documentation, scripts, and resource files (e.g., `main_menu_bar.md`, `folder_structure.md`).
- **Avoid spaces and special characters** in filenames.
- **Use descriptive names** that reflect the file's purpose or contents.

---

## Source Code Files
- Group related files by subsystem or feature (e.g., `src/ui/`, `src/ecs/`).

---

## Documentation Files
- All documentation files in the `docs/` folder should use lowercase and underscores (e.g., `file_naming.md`, `folder_structure.md`).
- Use the `.md` extension for Markdown files.

---

## Exceptions
- Third-party libraries and dependencies may use their own naming conventions. But these conventions are still preferred but not enforced.

---

## Prefixes
- Interface classes should use an 'I' prefix (e.g., IRenderer) and the file should match (e.g., IRenderer.h). Currently, 'I' for interfaces is the only prefix in use.
- **Concrete classes ('C') or Singletons 'S' are discouraged.**
- Use descriptive names for all other classes (e.g., WindowManager, Shader, Canvas).
- If you introduce a new naming convention, document it here and ensure it is justified and consistently applied.

---

For questions or suggestions, please contact a maintainer. 