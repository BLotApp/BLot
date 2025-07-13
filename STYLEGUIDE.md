# BLot Style Guide

## Header Files
- Use `#pragma once` at the very top of every header file
- Do **not** use `#ifndef`/`#define` include guards

## Indentation
- Use **tabs** for indentation throughout all source and header files.
- Do **not** use spaces for indentation.

## General
- Keep includes ordered: standard library, third-party, then project headers.
- Prefer modern C++ (C++17 or later) features and idioms.
- Keep code clean and minimal; remove unused includes and code.
- Document any API with brief comments.

## C++ Style Rules
- Always use explicit casts for type conversions, especially for narrowing conversions and enum usage.

## UI and Icons
- **Do not use emoticons** (😀, 🌙, ☀️, 🎨, etc.) in UI text or code comments
- Use FontAwesome icons from IconFontCppHeaders instead (e.g., `ICON_FA_MOON`, `ICON_FA_SUN`, `ICON_FA_PAINT_BRUSH`)
- FontAwesome icons provide consistent, professional appearance and better accessibility

---

*This style guide is enforced for all contributions to the BLot codebase.* 