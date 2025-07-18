# Core Managers in Blot

This document outlines the main manager classes in the Blot framework, their responsibilities, and how to access and use them.

## Access Pattern
- All managers are owned by the main engine class (`BlotEngine`).
- Access managers via the engine instance:

```cpp
blot::BlotEngine* engine = ...;
blot::ECSManager* ecs = engine->getECSManager();
blot::AddonManager* addons = engine->getAddonManager();
blot::UIManager* ui = engine->getUIManager();
blot::RenderingManager* rendering = engine->getRenderingManager();
blot::CanvasManager* canvas = engine->getCanvasManager();
blot::SettingsManager* settings = engine->getSettingsManager();
```

## Manager List

### ECSManager
- **Purpose:** Entity-Component-System (ECS) registry and logic.
- **Access:** `engine->getECSManager()`

### AddonManager
- **Purpose:** Manages discovery, loading, and lifecycle of addons.
- **Access:** `engine->getAddonManager()`

### UIManager
- **Purpose:** Handles UI logic, ImGui integration, and window management.
- **Access:** `engine->getUIManager()`

### CanvasManager
- **Purpose:** Manages canvases, their creation, and switching.
- **Access:** `engine->getCanvasManager()`
- **Note:** Internally uses `RenderingManager` for rendering operations.

### RenderingManager
- **Purpose:** Manages renderers, graphics, and low-level rendering logic. Used by `CanvasManager` and available for advanced use cases.
- **Access:** `engine->getRenderingManager()`

### SettingsManager
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