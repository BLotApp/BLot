#pragma once

#include "imgui.h"

namespace blot {

// ImVec2 operator overloads for vector math
inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { 
    return ImVec2(a.x + b.x, a.y + b.y); 
}

inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { 
    return ImVec2(a.x - b.x, a.y - b.y); 
}

inline ImVec2 operator*(const ImVec2& a, float s) { 
    return ImVec2(a.x * s, a.y * s); 
}

inline ImVec2 operator/(const ImVec2& a, float s) { 
    return ImVec2(a.x / s, a.y / s); 
}

} // namespace blot 