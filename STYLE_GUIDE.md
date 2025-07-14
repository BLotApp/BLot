# BLot Style Guide

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

## C++ Style Rules
- Always use explicit casts for type conversions, especially for narrowing conversions and enum usage.

## UI and Icons
- **Do not use emoticons** (😀, 🌙, ☀️, 🎨, etc.) in UI text or code comments
- Do use FontAwesome icons from IconFontCppHeaders (e.g., `ICON_FA_PENCIL`, `ICON_FA_PEN`, `ICON_FA_PAINT_BRUSH`)
- FontAwesome icons provide consistent appearance and better accessibility

## Keyboard Shortcuts
- Always use `ImGuiKey_*` enums (e.g., `ImGuiKey_Tab`, `ImGuiKey_S`) for all keyboard shortcut registration and handling.
- Do **not** use `GLFW_KEY_*` for ImGui shortcut logic. This ensures compatibility with ImGui's key system and future-proofs the code. 

---

*This style guide is enforced for all contributions to the BLot codebase.* 