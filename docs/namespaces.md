# Namespace Structure and Conventions

This document explains the namespace organization used in the Blot codebase and provides guidance for contributors.

## Top-Level Namespace
- All core engine code is placed in the `blot` namespace.
- This helps avoid name collisions and makes it clear which code belongs to the engine.

## Common Sub-Namespaces
- `blot::ecs` — ECS component definitions
- `blot::ecs` — ECS systems and related functions
- `blot::ui` — UI-related classes and functions
- `blot::rendering` — Rendering-related classes and functions
- (Add more as needed for new subsystems)

## Guidelines for Contributors
- Place new code in the most appropriate namespace.
- If your code is a new ECS component, use `blot::ecs`.
- If your code is a new ECS system, use `blot::ecs`.
- For UI or rendering code, use the corresponding sub-namespace.
- Avoid polluting the global namespace.
- If you need a new sub-namespace, document it here for consistency.

## Example
```cpp
namespace blot {
namespace ecs {
    void SShapeRendering(...);
}
namespace ecs {
    struct CTransform { ... };
}
}
```

---

Following these conventions helps keep the codebase organized, discoverable, and maintainable. 
