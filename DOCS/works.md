# Works Directory

The `works/` directory is a collection of creative coding demos, experiments, and sample applications built on top of the Blot framework. Each work is a self-contained project that demonstrates a specific feature, rendering technique, or creative idea.

## Structure

- Each work lives in its own subdirectory under `works/` (e.g., `works/sample_blend2d_gradient/`).
- Each work contains at least a `work.cpp` file (the entry point) and a `CMakeLists.txt` for building.
- The top-level `works/CMakeLists.txt` includes all subdirectories, so new works are automatically built when added.
- All works share the same core framework (ECS, rendering, UI, etc.), keeping the codebase maintainable and modular.

## Adding a New Work

1. Create a new subdirectory under `works/` (e.g., `works/my_new_demo/`).
2. Add a `work.cpp` file with your demo code.
3. Add a `CMakeLists.txt` to build your work as an executable.
4. (Optional) Specify which addons or features your work uses.
5. Your work will be built automatically when you build the project.

## Framework Integration

- All works are designed to use a shared framework, which provides:
  - Entity-Component-System (ECS) architecture
  - Rendering backends (OpenGL, Blend2D, etc.)
  - UI and addon management
- Works can load different addons or configurations as needed, keeping dependencies minimal.
- The framework lives in `src/framework/` (or `src/app/` for now) and is linked to each work.

## Example

See `works/sample_blend2d_gradient/` for a minimal example using Blend2D.

## Benefits

- **Modularity:** Each work is independent and easy to experiment with.
- **Maintainability:** Shared framework code means less duplication and easier updates.
- **Scalability:** Add as many works as you like, each with its own focus or set of addons.

---

For questions or suggestions, please contact a maintainer. 

# How to Build a Work

Each work in the `works/` directory is a self-contained project that uses the shared framework. To build a work:

1. Make sure the main project is configured with CMake (from the project root):
   ```sh
   cmake -S . -B build
   ```
2. Build the desired work (from the build directory):
   ```sh
   cmake --build build --target sample_blend2d_gradient
   ```
   Replace `sample_blend2d_gradient` with the name of your work's target.
3. Run the work's executable from the build output directory:
   ```sh
   ./works/sample_blend2d_gradient/sample_blend2d_gradient
   ```

## Framework Usage in Works

- Each work implements the `IWork` interface and is run by a minimal `main.cpp`.
- The framework provides access to ECS, rendering, and addon management via the `FrameworkApp` pointer passed to `setup()`.
- See the sample work for an example of ECS, rendering, and addon usage.

---

For more details, see the comments in the sample work or contact a maintainer. 