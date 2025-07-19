# Core Managers in Blot

This document outlines the main manager classes in the Blot framework, their responsibilities, and how to access and use them.

## Access Pattern
- All managers are owned by the main engine class (`BlotEngine`).
- Access managers via the engine instance:

```cpp
blot::BlotEngine* engine = ...;
blot::MEcs* ecs = engine->getECSManager();
blot::MAddon* addons = engine->getAddonManager();
blot::Mui* ui = engine->getUIManager();
blot::MRendering* rendering = engine->getRenderingManager();
blot::MCanvas* canvas = engine->getCanvasManager();
blot::MSettings* settings = engine->getSettingsManager();
```

### Inside `blot::IApp` subclasses
When you derive from `blot::IApp`, the framework automatically injects an engine pointer before `setup()` is called.  The base `IApp` class then exposes convenience accessors so you can use the same managers without boiler-plate:

```cpp
class MyCoolApp : public blot::IApp {
    void setup(blot::BlotEngine* engine) override {
        // No need to cache the engine – the base class already did.
        auto* ecs       = getECSManager();
        auto* ui        = getUIManager();
        auto* rendering = getRenderingManager();
        // ...
    }
    // ...
};
```

These helpers (`getECSManager()`, `getAddonManager()`, etc.) are protected members of `IApp`, keeping the public surface of your app class clean while still granting easy access to core managers.

Outside of an `IApp` subclass (e.g. in standalone utilities or engine components), continue to obtain managers through an explicit `blot::BlotEngine*` pointer as shown above.

## Manager List

### ECS Manager
- **Purpose:** Entity-Component-System (ECS) registry and logic.
- **Access:** `engine->getECSManager()`

### Addon Manager
- **Purpose:** Manages discovery, loading, and lifecycle of addons.
- **Access:** `engine->getAddonManager()`

### UI Manager
- **Purpose:** Handles UI logic, ImGui integration, and window management.
- **Access:** `engine->getUIManager()`

### Canvas Manager (MCanvas)
- **Purpose:** Manages canvases, their creation, and switching.
- **Access:** `engine->getCanvasManager()`
- **Note:** Internally uses `MRendering` for rendering operations.

### Rendering Manager
- **Purpose:** Manages renderers, graphics, and low-level rendering logic. Used by `MCanvas` and available for advanced use cases.
- **Access:** `engine->getRenderingManager()`

### Settings Manager
- **Purpose:** Handles application settings (load/save/configuration).
- **Access:** `engine->getSettingsManager()`

## Example Usage

```cpp
// Accessing managers from within a class with an engine pointer
void MyClass::doSomething() {
    auto* ecs = m_engine->getECSManager();
    auto* addons = m_engine->getAddonManager();
    // ...
}
```

---

**Note:**
- If you need a manager in a class, pass a pointer/reference to it or to the engine.
- This pattern improves testability and flexibility compared to singletons. 
