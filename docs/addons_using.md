# Using Framework Features in Addons

This guide explains how to use core framework features from within an addon, including the ECS system, event system, and more.

## Using the ECS System in Addons

Addons can interact with the core ECS system to create, modify, or observe entities and components. This allows addons to extend the engine with new behaviors, data, or editor features.

### Accessing the ECS Manager

The core application exposes an `MEcs` instance. Your addon can receive a pointer or reference to this manager (e.g., via dependency injection, a service locator, or a registration callback).

```cpp
// Example: storing a pointer to ECS Manager in your addon
void MyAddon::setECSManager(blot::MEcs* ecs) {
    m_ecs = ecs;
}
```

### Defining Custom Components

Addons may define their own ECS components in their own directories. These should be plain structs, following the same conventions as core components:

```cpp
// In addons/myAddon/MyCustomComponent.h
struct MyCustomComponent {
    float myValue = 0.0f;
    // Optionally, add GetProperties() for editor support
    std::vector<sProp> GetProperties() {
        return {
            {0, "My Value", EPT_FLOAT, &myValue}
        };
    }
};
```

### Registering and Using Components

Addons can add their components to entities using the ECS manager or directly via the registry:

```cpp
auto entity = m_ecs->createEntity("myAddonEntity");
m_ecs->addComponent<MyCustomComponent>(entity, MyCustomComponent{42.0f});
```

### Editor Integration

If your component implements `GetProperties()`, it will be automatically supported by the property inspector/editor.

---

For more details, see the code in `addons/` and the `MAddon` and `MEcs` classes. 
