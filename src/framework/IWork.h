#pragma once
class FrameworkApp;
class IWork {
public:
    virtual ~IWork() = default;
    virtual void setup(FrameworkApp* framework) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void draw() = 0;
}; 