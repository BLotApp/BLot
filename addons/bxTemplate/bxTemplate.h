#pragma once
#include <functional>
#include <string>
#include <unordered_map>
#include "core/IAddon.h"

class bxTemplate : public blot::IAddon {
  public:
	bxTemplate();
	~bxTemplate();

	bool init() override;
	void setup() override;
	void update(float deltaTime) override;
	void draw() override;
	void cleanup() override;

	// Parameter methods are now inherited from IAddon
	// No need to redeclare setParameter, getParameter, onParameterChanged
};
