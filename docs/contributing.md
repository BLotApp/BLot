# BLot Contributing Guide

## Header Files
- Use `#pragma once` at the very top of every header file
- Do **not** use `#ifndef`/`#define` include guards

## Indentation
- Use **tabs** for indentation throughout all source and header files.
- Do **not** use spaces for indentation.

**Data-Driven Design:**
- Favor approaches where behavior and configuration are controlled by data (e.g., config files, properties, or external resources) rather than hardcoded logic. This makes the codebase more flexible, maintainable, and adaptable to change. 
- **No hardcoded solutions**

## General
- Keep includes ordered: standard library, third-party, then project headers.
- Prefer modern C++ (C++17 or later) features and idioms.
- Keep code clean and minimal; remove unused includes and code.
- Document any API with brief comments.
- All core ECS component structs (used by code in `src/`) must be placed in `src/ecs/components/`.
- Addons and plugins may define and register their own ECS components in their own directories.

## C++ Style Rules
- Always use explicit casts for type conversions, especially for narrowing conversions and enum usage.

## UI and Icons
- **Do not use emoticons** (😀, 🌙, ☀️, 🎨, etc.) in UI text or code comments
- Do use FontAwesome icons from IconFontCppHeaders (e.g., `ICON_FA_PENCIL`, `ICON_FA_PEN`, `ICON_FA_PAINT_BRUSH`)
- FontAwesome icons provide consistent appearance and better accessibility

## Keyboard Shortcuts
- Always use `ImGuiKey_*` enums (e.g., `ImGuiKey_Tab`, `ImGuiKey_S`) for all keyboard shortcut registration and handling.
- Do **not** use `GLFW_KEY_*` for ImGui shortcut logic. This ensures compatibility with ImGui's key system and future-proofs the code. 

## File Naming Conventions
- Interface classes should use an 'I' prefix (e.g., IRenderer) and the file should match (e.g., IRenderer.h).
- Always keep class names and file names consistent.
- See [file_naming.md](file_naming.md) for more details.

## Git hooks (auto-format & newline)

To keep the codebase consistent we provide an **optional** Git pre-commit hook that:

1. runs `clang-format` on every staged `*.c`, `*.cpp`, `*.h`, `*.hpp` file,
2. guarantees every file ends with a single newline.

The hook lives under version-control at
`tools/hooks/pre-commit.sh` so you can inspect or modify it.

Install it once per clone:

```bash
# from the repository root
./tools/install-hooks.sh
```

The script copies the hook to `.git/hooks/pre-commit` and makes it
executable.  If `clang-format` isn’t on your PATH the formatting step is
skipped, but the newline check still runs.

Feel free to opt-out: simply remove or rename `.git/hooks/pre-commit` in
your local checkout.

---

*This contributing guide is enforced for all contributions to the BLot codebase.* 
