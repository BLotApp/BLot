#include "AppPaths.h"
#include <string>

namespace AppPaths {
std::string getAssetsDir() { return "assets"; }
std::string getFontsDir() { return getAssetsDir() + "/fonts"; }
std::string getWorkspacesDir() { return getAssetsDir() + "/user/workspaces"; }
std::string getUserSettingsFile() {
	return getAssetsDir() + "/user/blot_settings.json";
}
std::string getImGuiIniPath() { return getWorkspacesDir() + "/imgui.ini"; }
std::string getManifestPath() { return "app.json"; }
} // namespace AppPaths
