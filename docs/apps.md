# Apps Directory

The `apps/` directory is a collection of creative coding demos, experiments, and sample applications built on top of the Blot framework. Each app is a self-contained project that demonstrates a specific feature, rendering technique, or creative idea.

## Structure

- Each app lives in its own subdirectory under `apps/` (e.g., `apps/sample_blend2d_gradient/`).
- Each app contains at least `app.h`, `app.cpp`, and `app_entry.h` (the entry point with a `createApp()` factory function), plus a `CMakeLists.txt` for building.
- The top-level `apps/CMakeLists.txt` includes all subdirectories, so new apps are automatically built when added.
- All apps share the same core framework (ECS, rendering, UI, etc.), keeping the codebase maintainable and modular.

## Adding a New App

1. Create a new subdirectory under `apps/` (e.g., `apps/my_new_demo/`).
2. Add `app.h`, `app.cpp`, and `app_entry.h` with your app logic and factory function.
3. Add a `CMakeLists.txt` to build your app as an executable.
4. (Optional) Specify which addons or features your app uses.
5. Your app will be built automatically when you build the project.

## Framework Integration

- All apps are designed to use a shared framework, which provides:
  - Entity-Component-System (ECS) architecture
  - Rendering backends (OpenGL, Blend2D, etc.)
  - UI and addon management
- Apps can load different addons or configurations as needed, keeping dependencies minimal.
- The engine lives in `src/core/` and is linked to each app.

## Example

Minimal skeleton for any new app:

```cpp
#include "core/core.h"

class MyApp : public blot::IApp {
public:
    MyApp() {
        window().title = "My Sketch";
        window().width = 1280;
        window().height = 720;
    }

    void setup()  override {}
    void update(float dt) override {}
    void draw()   override {}
};

int main() {
    auto app = std::make_unique<MyApp>();
    blot::BlotEngine engine(std::move(app));
    engine.run();
}
```

See `apps/sample_menubar/` for a UI-focused example and `apps/sample_app/` for a full-framework demo.

## Benefits

- **Modularity:** Each app is independent and easy to experiment with.
- **Maintainability:** Shared framework code means less duplication and easier updates.
- **Scalability:** Add as many apps as you like, each with its own focus or set of addons.

---

For questions or suggestions, please contact a maintainer. 

# How to Build an App

Each app in the `apps/` directory is a self-contained project that uses the shared framework. To build an app:

1. Make sure the main project is configured with CMake (from the project root):
   ```sh
   cmake -S . -B build
   ```
2. Build the desired app (from the build directory):
   ```sh
   cmake --build build --target sample_blend2d_gradient
   ```
   Replace `sample_blend2d_gradient` with the name of your app's target.
3. Run the app's executable from the build output directory:
   ```sh
   ./apps/sample_blend2d_gradient/sample_blend2d_gradient
   ```

## Framework Usage in Apps

- Each app implements the `IApp` interface and is run by a minimal `main.cpp` that calls `createApp()` from `app_entry.h`.
- The engine provides access to ECS, rendering, and addon management via the `BlotEngine` pointer passed to `setup()`.
- See the sample app for an example of ECS, rendering, and addon usage.

## Umbrella Headers and Modular Structure
- Apps should use umbrella headers (e.g., `#include "core/core.h"`, `#include "ui/ui.h"`) for framework access.
- This keeps includes clean and ensures modularity across all apps.

---

For more details, see the comments in the sample app or contact a maintainer. 