## Keyboard Shortcuts
- Always use `ImGuiKey_*` enums (e.g., `ImGuiKey_Tab`, `ImGuiKey_S`) for all keyboard shortcut registration and handling.
- Do **not** use `GLFW_KEY_*` for ImGui shortcut logic. This ensures compatibility with ImGui's key system and future-proofs the code. 