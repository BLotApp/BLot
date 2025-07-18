#pragma once

#include <string>
#include <memory>
#include "framework/AddonBase.h"

// Forward declaration
class TextEditor;

class bxCodeEditor : public AddonBase {
public:
    bxCodeEditor();
    ~bxCodeEditor();
    std::string getName() const override { return "bxCodeEditor"; }
    bool init() override { return true; }
    void setup() override {}
    void update(float) override {}
    void draw() override {}
    void cleanup() override {}
    
    void loadDefaultTemplate();
    std::string getCode() const;
    void setCode(const std::string& code);
    
private:
    std::unique_ptr<TextEditor> m_editor;
    std::string m_currentCode;
}; 