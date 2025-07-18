#pragma once
#include "core/json.h" // Or wherever your JSON type is defined

class ISettings {
public:
    virtual ~ISettings() = default;
    virtual blot::json getSettings() const = 0;
    virtual void setSettings(const blot::json& settings) = 0;
}; 