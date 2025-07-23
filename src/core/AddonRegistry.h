#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace blot {
class IAddon;

class AddonRegistry {
  public:
	using Factory = std::function<std::shared_ptr<IAddon>()>;
	static AddonRegistry &instance();
	void addFactory(const std::string &name, Factory f);
	std::shared_ptr<IAddon> create(const std::string &name) const;

  private:
	std::unordered_map<std::string, Factory> m_factories;
};

// Helper macro for addons to self-register a factory at static-init time
// DEPRECATED: Use MAddon::registerAddon() instead
#define BLOT_REGISTER_ADDON(ADDON_CLASS)                                       \
	namespace {                                                                \
	struct ADDON_CLASS##Registrar {                                            \
		ADDON_CLASS##Registrar() {                                             \
			blot::AddonRegistry::instance().addFactory(#ADDON_CLASS, []() {    \
				return std::make_shared<ADDON_CLASS>();                        \
			});                                                                \
		}                                                                      \
	};                                                                         \
	static ADDON_CLASS##Registrar ADDON_CLASS##_auto_reg;                      \
	}

} // namespace blot
