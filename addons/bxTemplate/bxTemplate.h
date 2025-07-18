#pragma once
#include <string>
#include <unordered_map>
#include <functional>
#include "AddonBase.h"

class bxTemplate : public AddonBase {
public:
    bxTemplate();
    ~bxTemplate();

    bool init();
    void setup();
    void update(float deltaTime);
    void draw();
    void cleanup();

    void setParameter(const std::string& name, float value);
    float getParameter(const std::string& name) const;
    void onParameterChanged(const std::string& name, std::function<void(float)> callback);

private:
    bool m_initialized;
    float m_time;
    std::unordered_map<std::string, float> m_parameters;
    std::unordered_map<std::string, std::function<void(float)>> m_callbacks;
}; 