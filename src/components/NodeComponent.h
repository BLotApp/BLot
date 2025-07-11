#pragma once
#include <vector>
#include <entt/entity/entity.hpp>

struct NodeComponent {
    entt::entity parent = entt::null;
    std::vector<entt::entity> children;
}; 