#pragma once
#include <unordered_map>
#include <memory>
#include "CanvasComponent.h"

using EntityID = int;

class CanvasManager {
public:
    EntityID addCanvas(int width, int height);
    void removeCanvas(EntityID id);
    CanvasComponent* getCanvas(EntityID id);
    void updateAll(float dt);
    void renderAll();
    const std::unordered_map<EntityID, CanvasComponent>& getAll() const { return canvases; }
private:
    EntityID nextId = 1;
    std::unordered_map<EntityID, CanvasComponent> canvases;
}; 