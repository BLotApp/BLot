#pragma once
#include <glm/glm.hpp>
#include "../PropertyReflection.h"

struct TransformComponent {
    glm::vec2 position{0.0f, 0.0f};
    float rotation = 0.0f; // radians
    glm::vec2 scale{1.0f, 1.0f};

    std::vector<sProp> GetProperties() {
        return {
            {0, "Position X", EPT_FLOAT, &position.x},
            {1, "Position Y", EPT_FLOAT, &position.y},
            {2, "Rotation", EPT_FLOAT, &rotation},
            {3, "Scale X", EPT_FLOAT, &scale.x},
            {4, "Scale Y", EPT_FLOAT, &scale.y}
        };
    }
}; 