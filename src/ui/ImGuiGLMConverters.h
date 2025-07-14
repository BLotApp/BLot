#pragma once
#include <glm/glm.hpp>
#include <imgui.h>

// GLM to ImGui
inline ImVec2 ToImVec2(const glm::vec2& v) { return ImVec2(v.x, v.y); }
inline ImVec4 ToImVec4(const glm::vec4& v) { return ImVec4(v.x, v.y, v.z, v.w); }

// ImGui to GLM
inline glm::vec2 ToGLMVec2(const ImVec2& v) { return glm::vec2(v.x, v.y); }
inline glm::vec4 ToGLMVec4(const ImVec4& v) { return glm::vec4(v.x, v.y, v.z, v.w); } 