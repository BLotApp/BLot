#pragma once
#include <glm/glm.hpp>
#include "../PropertyReflection.h"

struct SelectableComponent {
    bool selected = false;
    glm::vec4 highlightColor = glm::vec4(1.0f, 0.8f, 0.2f, 1.0f); // Optional: color when selected

    std::vector<sProp> GetProperties() {
        return {
            {0, "Selected", EPT_BOOL, &selected},
            {1, "Highlight R", EPT_FLOAT, &highlightColor.x},
            {2, "Highlight G", EPT_FLOAT, &highlightColor.y},
            {3, "Highlight B", EPT_FLOAT, &highlightColor.z},
            {4, "Highlight A", EPT_FLOAT, &highlightColor.w}
        };
    }
}; 