# Application Folder Structure

This document explains the organization of the application's folders and their purposes. Use this as a reference for navigating, extending, or packaging the project.

---

## Documentation Naming Convention
- All documentation files in the DOCS folder should use lowercase letters and underscores (e.g., `folder_structure.md`, `file_naming.md`) for consistency.

---

## Folder and Subfolder Overview

| Folder / Path               | Description                                                      |
|-----------------------------|------------------------------------------------------------------|
| `assets/`                   | Static and user-editable resources                               |
|     `fonts/`                | Font files for UI and rendering                                  |
|     `user/`                 | User-generated or user-editable data                             |
|         `workspaces/`       | Workspace configuration files (`.json`)                          |
| `addons/`                   | Modular addon/plugin system; each addon in its own folder        |
| `apps/`                     | Sample and user apps, each in its own subdirectory               |
| `src/`                      | All application source code, organized by subsystem/feature      |
|     `core/`                 | Core engine logic, interfaces, and main app/engine classes       |
|     `framework/`            | Addon base classes, managers, and shared framework code          |
|     `rendering/`            | Rendering backends, renderer interfaces, and managers            |
|     `ui/`                   | UI system, ImGui integration, windows, and UI Manager             |
|     `ecs/`                  | Entity-Component-System (ECS) implementation and components      |
|     `resources/`            | Shaders, vertex buffers, and resource management                 |
|     `scripting/`            | Scripting engine and related code                                |
| `third_party/`              | External libraries and dependencies                              |
| `DOCS/`                     | Project documentation                                            |
| `works/` (if present)       | Legacy or experimental apps (being migrated to `apps/`)          |

---

## Umbrella Headers and Modularity
- Major modules provide umbrella headers (e.g., `core/core.h`, `ui/ui.h`, `rendering/rendering.h`, `ecs/ecs.h`) to simplify includes and improve modularity.
- Always use project-root-relative includes (e.g., `#include "core/core.h"`).
- See `DOCS/includes.md` for more details on include conventions.
