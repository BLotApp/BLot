set(UI_HEADERS
    TextRenderer.h
    ImGuiRenderer.h
    ImGuiGLMConverters.h
    ImGuiUtils.h
) 

# Ensure C++17 for filesystem
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add include directory for nlohmann/json
include_directories(${CMAKE_SOURCE_DIR}/third_party/json/include)

# Remove target_link_libraries for nlohmann_json if not needed
# target_link_libraries(ui PRIVATE nlohmann_json::nlohmann_json) 