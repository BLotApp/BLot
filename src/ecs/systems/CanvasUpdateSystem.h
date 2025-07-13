#pragma once
#include "ecs/ECSManager.h"
#include "canvas/Canvas.h"
#include <unordered_map>
#include <memory>
#include <entt/entity/entity.hpp>

// System function declaration
void CanvasUpdateSystem(ECSManager& ecs, ResourceManager* resourceManager, float deltaTime); 