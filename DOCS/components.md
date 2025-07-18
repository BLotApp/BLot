# Creating New ECS Components

This document explains how to create new components for the ECS (Entity-Component-System) architecture in this project, and how to make them compatible with the property inspector/editor.

## 1. Basic Component Structure

A component is a simple struct.
- If it is used by the core application (in `src/`), it must be placed in `src/ecs/components/`.
- Addons and plugins may define their own components in their own directories, but should follow the same conventions and (optionally) support property reflection for editor integration.

```cpp
struct MyComponent {
    float value = 0.0f;
    int count = 0;
};
```

## 2. Enabling Property Reflection (for Editor Support)

To make your component editable in the property window/editor, add a `GetProperties()` method that returns a `std::vector<sProp>`. This allows the editor to automatically detect and display your component's properties.

Example:

```cpp
#include "PropertyReflection.h" // or wherever sProp is defined

struct MyComponent {
    float value = 0.0f;
    int count = 0;

    std::vector<sProp> GetProperties() {
        return {
            {0, "Value", EPT_FLOAT, &value},
            {1, "Count", EPT_INT, &count}
        };
    }
};
```

- Use the appropriate `propTypes` enum for the `type` field (e.g., `EPT_FLOAT`, `EPT_INT`).
- The `data` field should point to the member variable.

## 3. What If You Don't Add GetProperties?

If your component does **not** implement `GetProperties()`, the property inspector will log a warning and skip it. This helps you catch missing editor support during development.

## 4. Registering the Component with ECS

Add your component to the ECS as usual. No inheritance or macros are required for property reflection—just the `GetProperties()` method.

## 5. Example: Full Component

```cpp
struct ExampleComponent {
    bool enabled = true;
    float threshold = 0.5f;

    std::vector<sProp> GetProperties() {
        return {
            {0, "Enabled", EPT_BOOL, &enabled},
            {1, "Threshold", EPT_FLOAT, &threshold}
        };
    }
};
```

## 6. Advanced: Type Safety

For more advanced use cases, consider using `std::variant` or a similar type-safe approach for the `data` field in `sProp`.

## Component Header Inclusion Best Practices

- **Never include component headers in general engine, manager, or utility headers.**
- **Only include component headers in system headers** (and their implementations) where the system API explicitly operates on those components.
- If a header only needs a pointer or reference to a type, prefer a forward declaration.
- **Umbrella headers** (headers that include many components, like the now-removed `ecs/ecs.h`) should not be used, as they leak dependencies and slow down builds.
- **Exception:** System headers are allowed to include component headers, as their purpose is to process those components directly.

---

**Summary:**
- Components are plain structs.
- All core ECS component structs (used by code in `src/`) must be placed in `src/ecs/components/`.
- Addons and plugins may define and register their own ECS components in their own directories.
- Add `GetProperties()` to enable property editing in the editor.
- If missing, a warning is logged.
- No inheritance or macros required. 