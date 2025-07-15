#pragma once
#include "Window.h"
#include "../WindowManager.h"
#include <memory>
#include <vector>
#include <string>

namespace blot {

class WindowManagerPanel : public Window {
public:
    WindowManagerPanel(const std::string& title, WindowManager* windowManager, Flags flags = Flags::None);

protected:
    void renderContents() override;

private:
    WindowManager* m_windowManager;
};

} // namespace blot 