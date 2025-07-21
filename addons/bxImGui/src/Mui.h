#pragma once

#include <deque>
#include <entt/entt.hpp>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "core/ISettings.h"
#include "ImGuiRenderer.h"
#include "MShortcut.h"
#include "MWindow.h"
#include "ui/windows/CanvasWindow.h"
#include "ui/windows/InfoWindow.h"
#include "ui/windows/PropertiesWindow.h"
#include "ui/windows/TextureViewerWindow.h"
#include "ui/windows/ThemePanel.h"
#include "ui/windows/ToolbarWindow.h"

// Forward declarations
struct GLFWwindow;
#include "CoordinateSystem.h"
#include "core/BlotEngine.h"
#include "ui/windows/SaveWorkspaceDialog.h"

namespace blot {
class MainMenuBar;
}

#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include "rendering/U_gladGlfw.h"
#include "U_ui.h"

// Forward declarations
namespace blot {
class SaveWorkspaceDialog;
class Window;
} // namespace blot

namespace blot {

// Notification types
enum class NotificationType { Info, Success, Warning, Error };

struct Notification {
	std::string message;
	NotificationType type;
	float timeRemaining;
};

struct Modal {
	std::string title;
	std::string message;
	NotificationType type;
	std::function<void()> onOk;
	bool open = true;
};

class Mui : public Iui {
  public:
	Mui(GLFWwindow *window);
	~Mui();

	void init() override;
	void shutdown() override;

	// Main UI operations
	void update() override;
	void handleInput() override;
	void render() override;

	// ImGui initialization and shutdown
	void initImGui();
	void shutdownImGui();

	// Window management
	void setupDockspace();
	void renderAllWindows();
	// Create default windows and wire their callbacks
	void setupWindows(BlotEngine *engine);
	void setupWindowCallbacks(BlotEngine *engine);

	// Window visibility management
	void setWindowVisibility(const std::string &windowName, bool visible);
	void setWindowVisibilityAll(bool visible);
	bool getWindowVisibility(const std::string &windowName) const;
	std::vector<std::string> getAllWindowNames() const;

	// Convenience wrappers around MWindow for easier access from apps
	std::shared_ptr<Window> getWindow(const std::string &windowName) {
		return m_windowManager ? m_windowManager->getWindow(windowName)
							   : nullptr;
	}

	// Register a custom window with the UI
	entt::entity addWindow(const std::string &windowName,
						   std::shared_ptr<Window> window) {
		return m_windowManager
				   ? m_windowManager->createWindow(windowName, window)
				   : entt::null;
	}

	// Workspace management (now via MWindow)
	bool loadWorkspace(const std::string &workspaceName);
	bool saveWorkspace(const std::string &workspaceName);
	bool saveWorkspaceAs(const std::string &workspaceName);
	std::string getCurrentWorkspace() const;
	std::vector<std::pair<std::string, std::string>>
	getAvailableWorkspaces() const;
	std::vector<std::string> getAllWorkspaceNames() const;
	void saveCurrentImGuiLayout();

	// Getters for external access
	MWindow *getWindowManager() { return m_windowManager.get(); }
	MShortcut &getShortcutManager() { return m_shortcutManager; }
	BlotEngine *getBlotEngine() const { return m_blotEngine; }

	// Templated window getters for type-safe access
	template <typename T>
	std::shared_ptr<T> getWindowAs(const std::string &name) {
		return m_windowManager->getWindowAs<T>(name);
	}

	template <typename T> std::shared_ptr<T> getFocusedWindowAs() {
		return m_windowManager->getFocusedWindowAs<T>();
	}

	// Text renderer access
	ImGuiRenderer *getImGuiRenderer() { return m_imguiRenderer.get(); }

	// Save workspace dialog access
	SaveWorkspaceDialog *getSaveWorkspaceDialog() {
		return m_saveWorkspaceDialog.get();
	}

	// Global ImGui theme for the entire UI
	enum class ImGuiTheme { Dark, Light, Classic, Corporate, Dracula };
	ImGuiTheme m_currentTheme = ImGuiTheme::Light;
	void setImGuiTheme(ImGuiTheme theme);
	ImGuiTheme getImGuiTheme() const { return m_currentTheme; }

	// Theme file management
	std::string m_lastThemePath = "themes/default.json";
	void saveCurrentTheme(const std::string &path);
	void loadTheme(const std::string &path);

	MainMenuBar *getMainMenuBar() { return m_mainMenuBar.get(); }

	// Notification/Popup API
	void showNotification(const std::string &message,
						  NotificationType type = NotificationType::Info,
						  float duration = 3.0f);
	void showModal(const std::string &title, const std::string &message,
				   NotificationType type = NotificationType::Info,
				   std::function<void()> onOk = nullptr);

	void setBlotEngine(BlotEngine *engine) { m_blotEngine = engine; }

	CoordinateSystem &getCoordinateSystem() { return m_coordinateSystem; }
	const CoordinateSystem &getCoordinateSystem() const {
		return m_coordinateSystem;
	}

	// ISettings interface
	json getSettings() const override;
	void setSettings(const json &settings) override;

  private:
	// GLFW window reference
	GLFWwindow *m_window;

	// Core window manager
	std::unique_ptr<MWindow> m_windowManager;

	// Save workspace dialog (still managed as unique_ptr, but registered with
	// MWindow)
	std::unique_ptr<SaveWorkspaceDialog> m_saveWorkspaceDialog;

	// Hide all windows except menubar flag
	bool m_bHideWindows = false;
	bool m_bHideMainMenuBar = false;

	// Shortcut manager
	MShortcut m_shortcutManager;

	// ImGui with enhanced text rendering
	std::unique_ptr<ImGuiRenderer> m_imguiRenderer;

	// Setup methods
	void configureWindowSettings();
	std::unique_ptr<MainMenuBar> m_mainMenuBar;

	std::deque<Notification> m_notifications;
	std::deque<Modal> m_modals;

	BlotEngine *m_blotEngine = nullptr;
	CoordinateSystem m_coordinateSystem;
};

} // namespace blot
