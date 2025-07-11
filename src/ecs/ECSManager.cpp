#include "ecs/ECSManager.h"
#include "rendering/Graphics.h"
#include "scripting/ScriptEngine.h"
#include <iostream>
#include <algorithm>
#include <entt/entt.hpp>

ECSManager::ECSManager() {
}

ECSManager::~ECSManager() {
    clear();
}

entt::entity ECSManager::createEntity(const std::string& name) {
    auto entity = m_registry.create();
    m_entities.push_back(entity);
    
    if (!name.empty()) {
        m_namedEntities[name] = entity;
    }
    
    return entity;
}

void ECSManager::destroyEntity(entt::entity entity) {
    // Remove from named entities
    for (auto it = m_namedEntities.begin(); it != m_namedEntities.end(); ++it) {
        if (it->second == entity) {
            m_namedEntities.erase(it);
            break;
        }
    }
    
    // Remove from entities list
    auto it = std::find(m_entities.begin(), m_entities.end(), entity);
    if (it != m_entities.end()) {
        m_entities.erase(it);
    }
    
    m_registry.destroy(entity);
}

entt::entity ECSManager::findEntity(const std::string& name) {
    auto it = m_namedEntities.find(name);
    if (it != m_namedEntities.end()) {
        return it->second;
    }
    return entt::null;
}

void ECSManager::updateSystems(float deltaTime) {
    updateAnimationSystem(deltaTime);
    updateScriptSystem(deltaTime);
    updateParameterSystem();
}

void ECSManager::renderSystems() {
    renderShapeSystem();
}

void ECSManager::connectParameters(entt::entity source, const std::string& sourceParam,
                                 entt::entity target, const std::string& targetParam) {
    if (!m_registry.valid(source) || !m_registry.valid(target)) {
        return;
    }
    
    if (m_registry.all_of<Parameter>(source) && m_registry.all_of<Parameter>(target)) {
        auto& sourceParamComp = m_registry.get<Parameter>(source);
        auto& targetParamComp = m_registry.get<Parameter>(target);
        
        sourceParamComp.isConnected = true;
        sourceParamComp.connectedTo = target;
        targetParamComp.isConnected = true;
        targetParamComp.connectedTo = source;
        
        // Transfer value
        targetParamComp.value = sourceParamComp.value;
    }
}

void ECSManager::disconnectParameters(entt::entity source, const std::string& sourceParam) {
    if (!m_registry.valid(source)) {
        return;
    }
    
    if (m_registry.all_of<Parameter>(source)) {
        auto& sourceParamComp = m_registry.get<Parameter>(source);
        sourceParamComp.isConnected = false;
        sourceParamComp.connectedTo = entt::null;
    }
}

void ECSManager::createNode(const std::string& nodeType, float x, float y) {
    auto entity = createEntity("node_" + nodeType);
    
    // Add transform component
    Transform transform;
    transform.x = x;
    transform.y = y;
    addComponent<Transform>(entity, transform);
    
    // Add node component
    Node node;
    node.nodeType = nodeType;
    addComponent<Node>(entity, node);
    
    // Add default parameters based on node type
    if (nodeType == "oscillator") {
        Parameter freq;
        freq.name = "frequency";
        freq.value = 440.0f;
        freq.minValue = 20.0f;
        freq.maxValue = 20000.0f;
        addComponent<Parameter>(entity, freq);
        
        Parameter amp;
        amp.name = "amplitude";
        amp.value = 0.5f;
        amp.minValue = 0.0f;
        amp.maxValue = 1.0f;
        addComponent<Parameter>(entity, amp);
    } else if (nodeType == "filter") {
        Parameter cutoff;
        cutoff.name = "cutoff";
        cutoff.value = 1000.0f;
        cutoff.minValue = 20.0f;
        cutoff.maxValue = 20000.0f;
        addComponent<Parameter>(entity, cutoff);
        
        Parameter resonance;
        resonance.name = "resonance";
        resonance.value = 0.5f;
        resonance.minValue = 0.0f;
        resonance.maxValue = 1.0f;
        addComponent<Parameter>(entity, resonance);
    }
}

void ECSManager::connectNodes(entt::entity sourceNode, const std::string& output,
                             entt::entity targetNode, const std::string& input) {
    if (!m_registry.valid(sourceNode) || !m_registry.valid(targetNode)) {
        return;
    }
    
    if (m_registry.all_of<Node>(sourceNode) && m_registry.all_of<Node>(targetNode)) {
        auto& sourceNodeComp = m_registry.get<Node>(sourceNode);
        auto& targetNodeComp = m_registry.get<Node>(targetNode);
        
        // Add output to source node
        if (std::find(sourceNodeComp.outputs.begin(), sourceNodeComp.outputs.end(), output) 
            == sourceNodeComp.outputs.end()) {
            sourceNodeComp.outputs.push_back(output);
        }
        
        // Add input to target node
        if (std::find(targetNodeComp.inputs.begin(), targetNodeComp.inputs.end(), input) 
            == targetNodeComp.inputs.end()) {
            targetNodeComp.inputs.push_back(input);
        }
    }
}

void ECSManager::clear() {
    m_registry.clear();
    m_namedEntities.clear();
    m_entities.clear();
}

size_t ECSManager::getEntityCount() const {
    return m_entities.size();
}

std::vector<entt::entity> ECSManager::getAllEntities() const {
    return m_entities;
}

void ECSManager::setCanvas(std::shared_ptr<Canvas> canvas) {
    m_canvas = canvas;
}

void ECSManager::setGraphics(std::shared_ptr<Graphics> graphics) {
    m_graphics = graphics;
}

void ECSManager::setScriptEngine(std::shared_ptr<ScriptEngine> scriptEngine) {
    m_scriptEngine = scriptEngine;
}

void ECSManager::updateAnimationSystem(float deltaTime) {
    auto view = m_registry.view<Animation, Transform>();
    
    for (auto entity : view) {
        auto& animation = view.get<Animation>(entity);
        auto& transform = view.get<Transform>(entity);
        
        if (animation.isPlaying) {
            animation.currentTime += deltaTime;
            
            if (animation.currentTime >= animation.duration) {
                if (animation.loop) {
                    animation.currentTime = 0.0f;
                } else {
                    animation.isPlaying = false;
                }
            }
            
            // Apply animation curve
            float progress = animation.currentTime / animation.duration;
            float easedProgress = progress; // Default linear
            
            switch (animation.curve) {
                case Animation::EaseIn:
                    easedProgress = progress * progress;
                    break;
                case Animation::EaseOut:
                    easedProgress = 1.0f - (1.0f - progress) * (1.0f - progress);
                    break;
                case Animation::EaseInOut:
                    easedProgress = progress < 0.5f ? 
                        2.0f * progress * progress : 
                        1.0f - 2.0f * (1.0f - progress) * (1.0f - progress);
                    break;
                case Animation::Bounce:
                    easedProgress = 1.0f - std::cos(progress * 3.14159f * 2.0f) * std::exp(-progress * 3.0f);
                    break;
                case Animation::Elastic:
                    easedProgress = std::sin(progress * 3.14159f * 2.0f) * std::exp(-progress * 2.0f);
                    break;
            }
            
            // Apply to transform (example: animate position)
            transform.x = easedProgress * 100.0f; // Simple example
        }
    }
}

void ECSManager::updateScriptSystem(float deltaTime) {
    auto view = m_registry.view<Script>();
    
    for (auto entity : view) {
        auto& script = view.get<Script>(entity);
        
        if (script.isActive && m_scriptEngine) {
            // Execute script code
            // In a real implementation, you'd want to compile and execute the script
            // For now, we'll just update the script engine
            m_scriptEngine->update(deltaTime);
        }
    }
}

void ECSManager::renderShapeSystem() {
    auto view = m_registry.view<Transform, Shape, Style>();
    
    for (auto entity : view) {
        auto& transform = view.get<Transform>(entity);
        auto& shape = view.get<Shape>(entity);
        auto& style = view.get<Style>(entity);
        
        if (m_graphics) {
            // Set style
            m_graphics->setFillColor(style.fillR, style.fillG, style.fillB, style.fillA);
            m_graphics->setStrokeColor(style.strokeR, style.strokeG, style.strokeB, style.strokeA);
            m_graphics->setStrokeWidth(style.strokeWidth);
            
            // Draw shape based on type
            switch (shape.type) {
                case Shape::Circle:
                    m_graphics->drawCircle(transform.x, transform.y, shape.width);
                    break;
                case Shape::Rectangle:
                    m_graphics->drawRect(transform.x, transform.y, shape.width, shape.height);
                    break;
                case Shape::Ellipse:
                    m_graphics->drawEllipse(transform.x, transform.y, shape.width, shape.height);
                    break;
                case Shape::Triangle:
                    m_graphics->drawTriangle(transform.x, transform.y,
                                          transform.x + shape.width, transform.y,
                                          transform.x + shape.width * 0.5f, transform.y + shape.height);
                    break;
                case Shape::Line:
                    m_graphics->drawLine(transform.x, transform.y,
                                       transform.x + shape.width, transform.y + shape.height);
                    break;
            }
        }
    }
}

void ECSManager::updateParameterSystem() {
    auto view = m_registry.view<Parameter>();
    
    for (auto entity : view) {
        auto& parameter = view.get<Parameter>(entity);
        
        // Update connected parameters
        if (parameter.isConnected && m_registry.valid(parameter.connectedTo)) {
            if (m_registry.all_of<Parameter>(parameter.connectedTo)) {
                auto& connectedParam = m_registry.get<Parameter>(parameter.connectedTo);
                connectedParam.value = parameter.value;
            }
        }
    }
} 