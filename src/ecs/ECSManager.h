#pragma once

#include <entt/entt.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>

// Forward declarations
class Canvas;
class Graphics;
class ScriptEngine;

// Component definitions
struct Transform {
    float x = 0.0f, y = 0.0f;
    float rotation = 0.0f;
    float scaleX = 1.0f, scaleY = 1.0f;
};

struct Shape {
    enum Type {
        Circle,
        Rectangle,
        Ellipse,
        Triangle,
        Line,
        Custom
    };
    
    Type type = Circle;
    float width = 50.0f, height = 50.0f;
    std::string customPath;
};

struct Style {
    float fillR = 1.0f, fillG = 1.0f, fillB = 1.0f, fillA = 1.0f;
    float strokeR = 0.0f, strokeG = 0.0f, strokeB = 0.0f, strokeA = 1.0f;
    float strokeWidth = 1.0f;
    bool hasFill = true;
    bool hasStroke = true;
};

struct Animation {
    float duration = 1.0f;
    float currentTime = 0.0f;
    bool isPlaying = false;
    bool loop = false;
    
    // Animation curves
    enum Curve {
        Linear,
        EaseIn,
        EaseOut,
        EaseInOut,
        Bounce,
        Elastic
    };
    Curve curve = Linear;
};

struct Parameter {
    std::string name;
    float value = 0.0f;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    bool isConnected = false;
    entt::entity connectedTo = entt::null;
};

struct Node {
    std::string nodeType;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::unordered_map<std::string, float> parameters;
};

struct Script {
    std::string code;
    bool isActive = true;
    std::string language = "cpp";
};

class ECSManager {
public:
    ECSManager();
    ~ECSManager();
    
    // Entity management
    entt::entity createEntity(const std::string& name = "");
    void destroyEntity(entt::entity entity);
    entt::entity findEntity(const std::string& name);
    
    // Component management
    template<typename T>
    T& addComponent(entt::entity entity, const T& component = T{});
    
    template<typename T>
    T& getComponent(entt::entity entity);
    
    template<typename T>
    bool hasComponent(entt::entity entity);
    
    template<typename T>
    void removeComponent(entt::entity entity);
    
    // System management
    void updateSystems(float deltaTime);
    void renderSystems();
    
    // Query systems
    template<typename... Components>
    auto view();
    
    // Parameter patching
    void connectParameters(entt::entity source, const std::string& sourceParam,
                         entt::entity target, const std::string& targetParam);
    void disconnectParameters(entt::entity source, const std::string& sourceParam);
    
    // Node editor integration
    void createNode(const std::string& nodeType, float x, float y);
    void connectNodes(entt::entity sourceNode, const std::string& output,
                     entt::entity targetNode, const std::string& input);
    
    // Utility functions
    void clear();
    size_t getEntityCount() const;
    std::vector<entt::entity> getAllEntities() const;
    
    // Integration with other systems
    void setCanvas(std::shared_ptr<Canvas> canvas);
    void setGraphics(std::shared_ptr<Graphics> graphics);
    void setScriptEngine(std::shared_ptr<ScriptEngine> scriptEngine);
    
private:
    entt::registry m_registry;
    std::unordered_map<std::string, entt::entity> m_namedEntities;
    std::vector<entt::entity> m_entities;
    
    // System references
    std::shared_ptr<Canvas> m_canvas;
    std::shared_ptr<Graphics> m_graphics;
    std::shared_ptr<ScriptEngine> m_scriptEngine;
    
    // Systems
    void updateAnimationSystem(float deltaTime);
    void updateScriptSystem(float deltaTime);
    void renderShapeSystem();
    void updateParameterSystem();
};

// Template implementations
template<typename T>
T& ECSManager::addComponent(entt::entity entity, const T& component) {
    return m_registry.emplace<T>(entity, component);
}

template<typename T>
T& ECSManager::getComponent(entt::entity entity) {
    return m_registry.get<T>(entity);
}

template<typename T>
bool ECSManager::hasComponent(entt::entity entity) {
    return m_registry.all_of<T>(entity);
}

template<typename T>
void ECSManager::removeComponent(entt::entity entity) {
    m_registry.remove<T>(entity);
}

template<typename... Components>
auto ECSManager::view() {
    return m_registry.view<Components...>();
} 