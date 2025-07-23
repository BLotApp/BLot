#include "core/AddonRegistry.h"

namespace blot {
AddonRegistry &AddonRegistry::instance() {
	static AddonRegistry inst;
	return inst;
}

void AddonRegistry::addFactory(const std::string &name, Factory f) {
	m_factories[name] = std::move(f);
}

std::shared_ptr<IAddon> AddonRegistry::create(const std::string &name) const {
	auto it = m_factories.find(name);
	return it == m_factories.end() ? nullptr : it->second();
}
} // namespace blot
