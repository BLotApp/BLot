#pragma once

#include <functional>
#include <imgui.h>
#include <memory>
#include "Window.h"
#include "ecs/MEcs.h"

// Forward declarations
class MEcs;

namespace blot {

class PropertiesWindow : public Window {
  public:
	PropertiesWindow(const std::string &title = "Properties",
					 Flags flags = Flags::None);
	virtual ~PropertiesWindow() = default;

	// Properties management
	void setECSManager(std::shared_ptr<MEcs> ecs);
	void setSelectedEntity(uint32_t entity);
	uint32_t getSelectedEntity() const;

	// Callbacks
	void setOnEntitySelected(std::function<void(uint32_t)> callback);
	void setOnPropertyChanged(
		std::function<void(uint32_t, const std::string &, const std::string &)>
			callback);

  private:
	std::shared_ptr<MEcs> m_ecs;
	uint32_t m_selectedEntity = 0;

	// Callbacks
	std::function<void(uint32_t)> m_onEntitySelected;
	std::function<void(uint32_t, const std::string &, const std::string &)>
		m_onPropertyChanged;

	// Helper methods
	void renderEntityList();
	void renderTransformProperties();
	void renderShapeProperties();
	void renderStyleProperties();
	bool renderColorPicker(const char *label, ImVec4 &color);
	void renderVector2Editor(const char *label, float &x, float &y);
	void renderFloatEditor(const char *label, float &value, float min = 0.0f,
						   float max = 100.0f);
	void renderIntEditor(const char *label, int &value, int min = 0,
						 int max = 100);
	void renderContents() override;
	void renderAllComponentProperties();
};

} // namespace blot
