#include "CanvasManager.h"

EntityID CanvasManager::addCanvas(int width, int height) {
    EntityID id = nextId++;
    canvases.emplace(id, CanvasComponent(width, height));
    return id;
}

void CanvasManager::removeCanvas(EntityID id) {
    canvases.erase(id);
}

CanvasComponent* CanvasManager::getCanvas(EntityID id) {
    auto it = canvases.find(id);
    if (it != canvases.end()) return &it->second;
    return nullptr;
}

void CanvasManager::updateAll(float dt) {
    for (auto& [id, comp] : canvases) {
        comp.canvas->update(dt);
    }
}

void CanvasManager::renderAll() {
    for (auto& [id, comp] : canvases) {
        comp.canvas->render();
    }
} 