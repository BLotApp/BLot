# Logging in Blot

Blot uses the popular **[spdlog](https://github.com/gabime/spdlog)** library for runtime logging, but most of the codebase does **not call spdlog directly**.  Instead, higher-level helpers encapsulate common patterns so that modules don’t need to worry about formatting or sinks.

## The `AddonBase::log()` Helper

Every add-on derives from `blot::AddonBase`.  That base class exposes
```cpp
void log(const std::string& message,
         spdlog::level::level_enum lvl = spdlog::level::info);
```
Internally it forwards to a central spdlog logger that is configured by the engine.  A typical add-on therefore only needs:
```cpp
this->log("Initializing foo-addon");
```
No `#include <spdlog/spdlog.h>` is required in the add-on’s source file (see `addons/bxTemplate/bxTemplate.cpp` for a minimal example).

## Why the indirection?
1. **Consistency** – messages are automatically prefixed with the add-on name.
2. **Decoupling** – add-ons do not depend on the logging backend; switching to another library would only touch the helper implementation.
3. **Configuration** – the engine sets log level, pattern, and sinks globally.

## Using spdlog Directly
It’s still possible (and sometimes useful) to include spdlog in engine/core code:
```cpp
#include <spdlog/spdlog.h>
spdlog::debug("deltaTime = {}", dt);
```
Just keep this out of add-on code unless you have a good reason.

## Where is the logger configured?
`src/core/U_core.h` and its implementation set up a default console sink with color support as soon as the engine starts.  Applications can add file sinks or change patterns before the first log call.

---
*Last updated: {{date}}* 
