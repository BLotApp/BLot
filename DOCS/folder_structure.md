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
| `src/`                      | All application source code, organized by subsystem/feature      |
|     `app/`                  | Application entry point, settings, main app logic                |
|     `addons/`               | Core addon/plugin system code                                    |
|     `canvas/`               | Canvas and canvas manager implementation                         |
|     `ecs/`                  | Entity-Component-System (ECS) core logic                         |
|         `components/`       | ECS component definitions                                        |
|         `systems/`          | ECS system implementations                                       |
|     `rendering/`            | Rendering backends, graphics utilities, renderer management      |
|     `resources/`            | Resource management (shaders, vertex buffers, etc.)              |
|     `scripting/`            | Scripting engine integration and logic                           |
|     `ui/`                   | User interface core logic, ImGui integration, UI utilities       |
|         `windows/`          | All ImGui-based window classes (menus, toolbars, dialogs, etc.)  |
| `addons/`                   | Built-in or user-created add-ons/plugins                         |
| `third_party/`              | External libraries and dependencies                              |
| `build/`                    | Compiled binaries and copied resources after building            |

---

## Best Practices
- **User Data:** Always store user-editable data in `assets/user/` to avoid overwriting during updates.
- **Static Assets:** Place fonts, images, and other static resources in `assets/` (but outside `user/`).
- **Addons:** Place new add-ons in the `addons/` directory.
- **Build Output:** Run the application from the build output directory to ensure all resources are found.

---

## Example: Adding a New Workspace
1. Save a new workspace in the app, or manually add a `.json` file to `assets/user/workspaces/`.
2. The workspace will appear in the Workspace > Load menu.

---

## Updating or Packaging the App
- Only update/replace files in `assets/` (except `assets/user/`) and `addons/`.
- Instruct users to back up `assets/user/` for their custom data.

---

For further details, see the code documentation or ask a maintainer. 