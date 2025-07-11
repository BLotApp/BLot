#pragma once
#include <glm/glm.hpp>

struct SelectableComponent {
    bool selected = false;
    glm::vec4 highlightColor = glm::vec4(1.0f, 0.8f, 0.2f, 1.0f); // Optional: color when selected
}; 