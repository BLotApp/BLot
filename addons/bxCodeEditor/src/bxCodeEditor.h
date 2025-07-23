#pragma once

#include <memory>
#include <string>
#include "core/IAddon.h"

// Forward declaration
class TextEditor;

class bxCodeEditor : public blot::IAddon {
  public:
	bxCodeEditor();
	~bxCodeEditor();
	// Inherit getName() from IAddon which already returns the addon name set
	// via constructor.
	bool init() override { return true; }
	void setup() override {}
	void update(float) override {}
	void draw() override {}
	void cleanup() override {}

	void loadDefaultTemplate();
	std::string getCode() const;
	void setCode(const std::string &code);

  private:
	std::unique_ptr<TextEditor> m_editor;
	std::string m_currentCode;
};
