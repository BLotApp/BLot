#include "core/canvas/CanvasManager.h"
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include "core/ISettings.h"
#include "ecs/ECSManager.h"
#include "ecs/components/CanvasComponent.h"
#include "ecs/components/TextureComponent.h"

namespace blot {

CanvasManager::CanvasManager(BlotEngine *engine)
	: m_activeCanvasIndex(0), m_engine(engine) {}

CanvasManager::~CanvasManager() { clear(); }

std::shared_ptr<Canvas>
CanvasManager::createCanvas(const CanvasSettings &settings,
							const std::string &name) {
	std::string canvasName = name.empty() ? generateDefaultName() : name;
	if (hasCanvas(canvasName)) {
		throw std::runtime_error("Canvas with name '" + canvasName +
								 "' already exists");
	}
	auto canvas = std::make_shared<Canvas>(settings, m_engine);
	canvas->setName(canvasName);
	m_canvases.push_back(canvas);
	size_t newIndex = m_canvases.size() - 1;
	if (newIndex == 0) {
		m_activeCanvasIndex = 0;
	}
	if (m_onCanvasCreated) {
		m_onCanvasCreated(newIndex, canvasName);
	}
	std::cout << "Created canvas: " << canvasName << " (index: " << newIndex
			  << ")" << std::endl;
	return canvas;
}

entt::entity CanvasManager::createCanvas(ECSManager &ecs, int width, int height,
										 const std::string &name) {
	entt::entity entity = ecs.createEntity(name);

	// Add CanvasComponent
	blot::components::CanvasComponent canvasComp;
	canvasComp.width = width;
	canvasComp.height = height;
	canvasComp.name = name;
	ecs.addComponent<blot::components::CanvasComponent>(entity, canvasComp);

	// Add TextureComponent (for framebuffer/texture)
	blot::components::TextureComponent texComp;
	texComp.width = width;
	texComp.height = height;
	texComp.renderTarget = true;
	ecs.addComponent<blot::components::TextureComponent>(entity, texComp);

	// Add other components as needed...

	return entity;
}

void CanvasManager::removeCanvas(size_t index) {
	validateIndex(index);

	std::string removedName = m_canvases[index]->getName();

	m_canvases.erase(m_canvases.begin() + index);

	// Adjust active canvas index if necessary
	if (m_canvases.empty()) {
		m_activeCanvasIndex = 0;
	} else if (m_activeCanvasIndex >= m_canvases.size()) {
		m_activeCanvasIndex = m_canvases.size() - 1;
	}

	// Trigger callback
	if (m_onCanvasRemoved) {
		m_onCanvasRemoved(index, removedName);
	}

	std::cout << "Removed canvas: " << removedName << " (index: " << index
			  << ")" << std::endl;
}

void CanvasManager::removeCanvas(const std::string &name) {
	size_t index = findCanvasIndex(name);
	if (index != static_cast<size_t>(-1)) {
		removeCanvas(index);
	} else {
		throw std::runtime_error("Canvas with name '" + name + "' not found");
	}
}

void CanvasManager::renameCanvas(size_t index, const std::string &newName) {
	validateIndex(index);
	validateName(newName);

	std::string oldName = m_canvases[index]->getName();
	m_canvases[index]->setName(newName);

	// Trigger callback
	if (m_onCanvasRenamed) {
		m_onCanvasRenamed(index, oldName, newName);
	}

	std::cout << "Renamed canvas " << index << " from '" << oldName << "' to '"
			  << newName << "'" << std::endl;
}

void CanvasManager::renameCanvas(const std::string &oldName,
								 const std::string &newName) {
	size_t index = findCanvasIndex(oldName);
	if (index != static_cast<size_t>(-1)) {
		renameCanvas(index, newName);
	} else {
		throw std::runtime_error("Canvas with name '" + oldName +
								 "' not found");
	}
}

void CanvasManager::setActiveCanvas(size_t index) {
	validateIndex(index);

	if (m_activeCanvasIndex != index) {
		m_activeCanvasIndex = index;

		std::string canvasName = m_canvases[index]->getName();

		// Trigger callback
		if (m_onActiveCanvasChanged) {
			m_onActiveCanvasChanged(index, canvasName);
		}

		std::cout << "Switched to canvas: " << canvasName
				  << " (index: " << index << ")" << std::endl;
	}
}

void CanvasManager::setActiveCanvas(const std::string &name) {
	size_t index = findCanvasIndex(name);
	if (index != static_cast<size_t>(-1)) {
		setActiveCanvas(index);
	} else {
		throw std::runtime_error("Canvas with name '" + name + "' not found");
	}
}

std::shared_ptr<Canvas> CanvasManager::getActiveCanvas() const {
	if (m_canvases.empty()) {
		return nullptr;
	}
	return m_canvases[m_activeCanvasIndex];
}

size_t CanvasManager::getActiveCanvasIndex() const {
	return m_activeCanvasIndex;
}

std::string CanvasManager::getActiveCanvasName() const {
	if (m_canvases.empty()) {
		return "";
	}
	return m_canvases[m_activeCanvasIndex]->getName();
}

std::vector<std::shared_ptr<Canvas>> CanvasManager::getAllCanvases() const {
	return m_canvases;
}

std::shared_ptr<Canvas> CanvasManager::getCanvas(size_t index) const {
	validateIndex(index);
	return m_canvases[index];
}

std::shared_ptr<Canvas>
CanvasManager::getCanvas(const std::string &name) const {
	size_t index = findCanvasIndex(name);
	if (index != static_cast<size_t>(-1)) {
		return m_canvases[index];
	}
	return nullptr;
}

size_t CanvasManager::getCanvasCount() const { return m_canvases.size(); }

size_t CanvasManager::findCanvasIndex(const std::string &name) const {
	for (size_t i = 0; i < m_canvases.size(); ++i) {
		if (m_canvases[i]->getName() == name) {
			return i;
		}
	}
	return static_cast<size_t>(-1);
}

bool CanvasManager::hasCanvas(const std::string &name) const {
	return findCanvasIndex(name) != static_cast<size_t>(-1);
}

std::vector<std::string> CanvasManager::getAllCanvasNames() const {
	std::vector<std::string> names;
	names.reserve(m_canvases.size());

	for (const auto &canvas : m_canvases) {
		names.push_back(canvas->getName());
	}

	return names;
}

std::vector<std::pair<size_t, std::string>>
CanvasManager::getAllCanvasInfo() const {
	std::vector<std::pair<size_t, std::string>> info;
	info.reserve(m_canvases.size());

	for (size_t i = 0; i < m_canvases.size(); ++i) {
		info.emplace_back(i, m_canvases[i]->getName());
	}

	return info;
}

void CanvasManager::clear() {
	size_t oldCount = m_canvases.size();
	m_canvases.clear();
	m_activeCanvasIndex = 0;

	std::cout << "Cleared " << oldCount << " canvases" << std::endl;
}

void CanvasManager::validateIndex(size_t index) const {
	if (index >= m_canvases.size()) {
		throw std::out_of_range("Canvas index " + std::to_string(index) +
								" is out of range");
	}
}

void CanvasManager::validateName(const std::string &name) const {
	if (name.empty()) {
		throw std::invalid_argument("Canvas name cannot be empty");
	}

	if (hasCanvas(name)) {
		throw std::runtime_error("Canvas with name '" + name +
								 "' already exists");
	}
}

std::string CanvasManager::generateDefaultName() const {
	size_t count = m_canvases.size();
	return "Canvas " + std::to_string(count);
}

blot::json CanvasManager::getSettings() const {
	blot::json j;
	j["activeCanvasIndex"] = m_activeCanvasIndex;
	j["canvases"] = blot::json::array();
	for (const auto &canvas : m_canvases) {
		blot::json cj = canvas->getSettings();
		cj["name"] = canvas->getName();
		j["canvases"].push_back(cj);
	}
	return j;
}

void CanvasManager::setSettings(const blot::json &settings) {
	clear();
	if (settings.contains("activeCanvasIndex"))
		m_activeCanvasIndex = settings["activeCanvasIndex"];
	if (settings.contains("canvases")) {
		for (const auto &cj : settings["canvases"]) {
			CanvasSettings cs;
			if (cj.contains("width"))
				cs.width = cj["width"];
			if (cj.contains("height"))
				cs.height = cj["height"];
			if (cj.contains("r"))
				cs.r = cj["r"];
			if (cj.contains("g"))
				cs.g = cj["g"];
			if (cj.contains("b"))
				cs.b = cj["b"];
			if (cj.contains("a"))
				cs.a = cj["a"];
			if (cj.contains("samples"))
				cs.samples = cj["samples"];
			std::string name = cj.value("name", "");
			auto canvas = std::make_shared<Canvas>(cs, m_engine);
			canvas->setSettings(cj);
			canvas->setName(name);
			m_canvases.push_back(canvas);
		}
	}
}

} // namespace blot
