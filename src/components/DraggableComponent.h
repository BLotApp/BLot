#pragma once
#include <glm/glm.hpp>

struct DraggableComponent {
    bool draggable = true;
    bool dragging = false;
    glm::vec2 dragOffset = glm::vec2(0.0f, 0.0f); // Offset from entity position to mouse when dragging
}; 