# Settings Serialization and Persistence in Blot

## Overview
Blot uses a consistent JSON-based settings system for all engine classes that support configuration or serialization. This is achieved via the `ISettings` interface, which enforces the implementation of `getSettings()` and `setSettings()` methods.

## The ISettings Interface
Any class that supports settings export/import should inherit from `ISettings`:

```cpp
class ISettings {
public:
    virtual ~ISettings() = default;
    virtual blot::json getSettings() const = 0;
    virtual void setSettings(const blot::json& settings) = 0;
};
```

## Saving Settings
To save the settings of an object (e.g., a Canvas or MEcs), call `getSettings()` and write the resulting JSON to disk:

```cpp
blot::json canvasJson = canvas->getSettings();
std::ofstream out("canvas.json");
out << canvasJson.dump(4);
```

To save the settings for the whole engine or a group of objects:

```cpp
blot::json engineJson;
engineJson["canvas"] = canvas->getSettings();
engineJson["ecs"] = ecsManager->getSettings();
// ... add more as needed
std::ofstream out("project.json");
out << engineJson.dump(4);
```

## Loading Settings
To load settings, read the JSON from disk and call `setSettings()`:

```cpp
std::ifstream in("canvas.json");
blot::json canvasJson;
in >> canvasJson;
canvas->setSettings(canvasJson);
```

For the whole engine:

```cpp
std::ifstream in("project.json");
blot::json engineJson;
in >> engineJson;
canvas->setSettings(engineJson["canvas"]);
ecsManager->setSettings(engineJson["ecs"]);
// ...
```

## ECS Components and Property Reflection
ECS components can use the property reflection system to automate settings serialization. See the components documentation for details.

## Benefits
- Human-readable, extensible, and versionable file format
- Consistent API for all engine classes
- Easy integration with editors, scripting, and automation

---

For more details, see the `ISettings` interface and example implementations in the codebase. 
