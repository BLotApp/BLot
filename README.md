# BLot

BLot (Pronounced Beelot) is a small creative coding app inspired by DrawBot and OpenFrameworks.

### Dependencies

| Library      | Purpose                        | How Managed   | Notes                                 |
|--------------|--------------------------------|---------------|---------------------------------------|
| GLFW3        | Window/input management        | vcpkg         | Native/system for Pi recommended      |
| FreeType     | Font/text rendering            | vcpkg         |                                       |
| GLM          | Math (vectors/matrices)        | submodule     | Header-only                           |
| GLAD         | OpenGL loader                  | vcpkg         |                                       |
| Dear ImGui   | UI framework                   | submodule     | Not managed by vcpkg                  |
| Blend2D      | 2D vector graphics             | vcpkg         | Native/system for Pi recommended      |
| EnTT         | ECS (Entity-Component-System)  | submodule     | Header-only                           |

### Prerequisites

- CMake 3.16 or higher
- C++17 compatible compiler
- vcpkg for dependency management

### Build Instructions

Use the build script.

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests if applicable
5. Submit a pull request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Inspired by [DrawBot](https://drawbot.com/) and [OpenFrameworks](https://openframeworks.cc/)
- Uses [Dear ImGui](https://github.com/ocornut/imgui) for UI
- Graphics powered by [OpenGL](https://www.opengl.org/) and [Blend2D](https://blend2d.com/)
- Entity-Component-System by [EnTT](https://github.com/skypjack/entt)
- Node editor integration with [imgui-node-editor](https://github.com/thedmd/imgui-node-editor)

## Development Credits

This project was developed with assistance from Claude Sonnet 4, an AI coding assistant.
