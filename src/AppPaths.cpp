#include "AppPaths.h"
#include <string>

namespace AppPaths {
    std::string getAssetsDir()      { return "assets"; }
    std::string getFontsDir()      { return getAssetsDir() + "/fonts"; }
    std::string getWorkspacesDir() { return getAssetsDir() + "/user/workspaces"; }
    std::string getImGuiIniPath()  { return getWorkspacesDir() + "/imgui.ini"; }
} 