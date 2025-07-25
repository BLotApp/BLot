#include "AddonRegistry.h"
#include "IAddon.h"

blot::AddonRegistry &blot::AddonRegistry::instance() {
	static AddonRegistry inst;
	return inst;
}

void blot::AddonRegistry::addFactory(const std::string &name, Factory f) {
	m_factories[name] = std::move(f);
}

std::shared_ptr<blot::IAddon> blot::AddonRegistry::create(const std::string &name) const {
	auto it = m_factories.find(name);
	return it == m_factories.end() ? nullptr : it->second();
} 