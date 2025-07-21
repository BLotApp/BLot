#include "MWindow.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <spdlog/spdlog.h>
#include <sstream>
#include "core/json.h"
#include "core/util/AppPaths.h"
#include "ecs/components/CTransform.h"
#include "ecs/components/CWindow.h"
#include "imgui.h"

namespace blot {

MWindow::MWindow() : m_focusedWindowEntity(entt::null) {
	m_workspaceDir = AppPaths::getWorkspacesDir();
	m_mainIniPath = AppPaths::getImGuiIniPath();
	spdlog::debug("[DEBUG] MWindow constructed, workspaceDir={}",
				  m_workspaceDir);
	loadExistingWorkspaces();
}

MWindow::~MWindow() { closeAllWindows(); }

entt::entity MWindow::createWindow(const std::string &name,
										 std::shared_ptr<Window> window) {
	// Check if window with this name already exists
	auto existingEntity = getWindowEntity(name);
	if (existingEntity != entt::null) {
		return existingEntity;
	}

	// Create new window entity
	auto entity = m_registry.create();

	// Add components
	m_registry.emplace<ecs::CWindow>(entity, window, name, true, false, 0);
	m_registry.emplace<ecs::CWindowTransform>(entity);
	m_registry.emplace<ecs::CWindowStyle>(entity);
	m_registry.emplace<ecs::CWindowInput>(entity);

	// If this is the first window, make it focused
	if (m_focusedWindowEntity == entt::null) {
		m_focusedWindowEntity = entity;
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isFocused = true;
	}

	return entity;
}

void MWindow::destroyWindow(entt::entity windowEntity) {
	if (m_registry.valid(windowEntity)) {
		// If we're destroying the focused window, clear focus
		if (windowEntity == m_focusedWindowEntity) {
			m_focusedWindowEntity = entt::null;
		}
		m_registry.destroy(windowEntity);
	}
}

void MWindow::destroyWindow(const std::string &windowName) {
	auto entity = getWindowEntity(windowName);
	if (entity != entt::null) {
		destroyWindow(entity);
	}
}

entt::entity MWindow::getWindowEntity(const std::string &name) {
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (windowComp.name == name) {
			return entity;
		}
	}
	return entt::null;
}

std::shared_ptr<Window> MWindow::getWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		return windowComp.window;
	}
	return nullptr;
}

std::shared_ptr<Window> MWindow::getFocusedWindow() {
	if (m_focusedWindowEntity != entt::null &&
		m_registry.valid(m_focusedWindowEntity)) {
		auto &windowComp =
			m_registry.get<ecs::CWindow>(m_focusedWindowEntity);
		return windowComp.window;
	}
	return nullptr;
}

entt::entity MWindow::getFocusedWindowEntity() {
	return m_focusedWindowEntity;
}

std::vector<std::string> MWindow::getAllWindowNames() const {
	std::vector<std::string> windowNames;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		const auto &windowComp = view.get<ecs::CWindow>(entity);
		windowNames.push_back(windowComp.name);
	}
	return windowNames;
}

std::vector<std::pair<std::string, std::string>>
MWindow::getAllWindowsWithDisplayNames() const {
	std::vector<std::pair<std::string, std::string>> windows;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		const auto &windowComp = view.get<ecs::CWindow>(entity);
		// Use the window's title for display, fallback to name if window is
		// null
		std::string displayName =
			windowComp.window ? windowComp.window->getTitle() : windowComp.name;
		windows.push_back({windowComp.name, displayName});
	}
	return windows;
}

void MWindow::showWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isVisible = true;
		if (windowComp.window) {
			windowComp.window->show();
		}
	}
}

void MWindow::hideWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isVisible = false;
		if (windowComp.window) {
			windowComp.window->hide();
		}
	}
}

void MWindow::closeWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		if (windowComp.window) {
			windowComp.window->close();
		}
		destroyWindow(entity);
	}
}

void MWindow::focusWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		// Clear previous focus
		if (m_focusedWindowEntity != entt::null &&
			m_registry.valid(m_focusedWindowEntity)) {
			auto &prevWindowComp =
				m_registry.get<ecs::CWindow>(m_focusedWindowEntity);
			prevWindowComp.isFocused = false;
		}

		// Set new focus
		m_focusedWindowEntity = entity;
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isFocused = true;
	}
}

void MWindow::closeFocusedWindow() {
	if (m_focusedWindowEntity != entt::null &&
		m_registry.valid(m_focusedWindowEntity)) {
		auto &windowComp =
			m_registry.get<ecs::CWindow>(m_focusedWindowEntity);
		if (windowComp.window) {
			windowComp.window->close();
		}
		destroyWindow(m_focusedWindowEntity);
		updateFocus();
	}
}

void MWindow::closeAllWindows() {
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (windowComp.window) {
			windowComp.window->close();
		}
	}
	m_registry.clear();
	m_focusedWindowEntity = entt::null;
}

void MWindow::renderAllWindows() {
	spdlog::debug("[MWindow] renderAllWindows() called");
	// Sort windows by z-order
	sortWindowsByZOrder();

	// Render all visible windows
	auto view = m_registry.view<ecs::CWindow, ecs::CWindowTransform,
								ecs::CWindowStyle>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		auto &transformComp = view.get<ecs::CWindowTransform>(entity);
		auto &styleComp = view.get<ecs::CWindowStyle>(entity);

		if (windowComp.isVisible && windowComp.window) {
			// Apply transform and style
			windowComp.window->setPosition(transformComp.position);
			windowComp.window->setSize(transformComp.size);
			windowComp.window->setMinSize(transformComp.minSize);
			windowComp.window->setMaxSize(transformComp.maxSize);
			windowComp.window->setAlpha(styleComp.alpha);
			windowComp.window->setFlags(
				static_cast<Window::Flags>(styleComp.flags));

			// Render the window
			windowComp.window->render();
		}
	}
}

void MWindow::handleInput() {
	// Handle ESC key to close focused window
	handleEscapeKey();

	// Update focus based on ImGui's focused window
	updateFocus();
}

void MWindow::update() {
	// Update all window states
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (windowComp.window) {
			// Update window state from ImGui
			windowComp.isFocused = windowComp.window->isFocused();
			windowComp.isVisible = windowComp.window->isVisible();
		}
	}

	// Handle input
	handleInput();
}

void MWindow::updateFocus() {
	// Find the currently focused window in ImGui
	entt::entity newFocusedEntity = entt::null;

	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (windowComp.window && windowComp.isFocused) {
			newFocusedEntity = entity;
			break;
		}
	}

	// Update focus state
	if (newFocusedEntity != m_focusedWindowEntity) {
		// Clear previous focus
		if (m_focusedWindowEntity != entt::null &&
			m_registry.valid(m_focusedWindowEntity)) {
			auto &prevWindowComp =
				m_registry.get<ecs::CWindow>(m_focusedWindowEntity);
			prevWindowComp.isFocused = false;
		}

		// Set new focus
		m_focusedWindowEntity = newFocusedEntity;
		if (newFocusedEntity != entt::null) {
			auto &windowComp =
				m_registry.get<ecs::CWindow>(newFocusedEntity);
			windowComp.isFocused = true;
		}
	}
}

void MWindow::handleEscapeKey() {
	if (ImGui::IsKeyPressed(ImGuiKey_Escape)) {
		// Find the focused window that should close on ESC
		auto view = m_registry.view<ecs::CWindow, ecs::CWindowInput>();
		for (auto entity : view) {
			auto &windowComp = view.get<ecs::CWindow>(entity);
			auto &inputComp = view.get<ecs::CWindowInput>(entity);

			if (windowComp.isFocused && inputComp.closeOnEscape) {
				closeFocusedWindow();
				break;
			}
		}
	}
}

void MWindow::sortWindowsByZOrder() {
	// This would sort windows by z-order for proper rendering
	// For now, we'll use the default entity order
	// In a more complex implementation, you might want to sort the view
}

// Window visibility management
bool MWindow::isWindowVisible(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		return windowComp.isVisible;
	}
	return false;
}

void MWindow::setWindowVisible(const std::string &name, bool visible) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isVisible = visible;
		if (windowComp.window) {
			if (visible) {
				windowComp.window->show();
			} else {
				windowComp.window->hide();
			}
		}
	}
}

void MWindow::toggleWindow(const std::string &name) {
	auto entity = getWindowEntity(name);
	if (entity != entt::null) {
		auto &windowComp = m_registry.get<ecs::CWindow>(entity);
		windowComp.isVisible = !windowComp.isVisible;
		if (windowComp.window) {
			if (windowComp.isVisible) {
				windowComp.window->show();
			} else {
				windowComp.window->hide();
			}
		}
	}
}

void MWindow::showAllWindows() {
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		windowComp.isVisible = true;
		if (windowComp.window) {
			windowComp.window->show();
		}
	}
}

void MWindow::hideAllWindows(const std::vector<std::string> &except) {
	auto allWindows = getAllWindowNames();
	for (const auto &name : allWindows) {
		if (std::find(except.begin(), except.end(), name) != except.end())
			continue;
		setWindowVisible(name, false);
	}
}

void MWindow::setMainMenuBar(bool visible) {
	setWindowVisible("MainMenuBar", visible);
}

// Window settings management
// Remove all usages of ecs::CWindowSettings, setWindowSettings, and
// getWindowSettings

std::vector<std::string> MWindow::getVisibleWindows() {
	std::vector<std::string> visibleWindows;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (windowComp.isVisible) {
			visibleWindows.push_back(windowComp.name);
		}
	}
	return visibleWindows;
}

std::vector<std::string> MWindow::getHiddenWindows() {
	std::vector<std::string> hiddenWindows;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		if (!windowComp.isVisible) {
			hiddenWindows.push_back(windowComp.name);
		}
	}
	return hiddenWindows;
}

std::vector<std::string>
MWindow::getWindowsByCategory(const std::string &category) {
	std::vector<std::string> categoryWindows;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		// The category is no longer stored in ecs::CWindow, so this will
		// always return empty This function needs to be updated to reflect the
		// new ecs::CWindow structure For now, returning an empty vector as a
		// placeholder
		categoryWindows.push_back(windowComp.name);
	}
	return categoryWindows;
}

// Menu integration
void MWindow::renderWindowMenu() {
	if (ImGui::BeginMenu("Windows")) {
		auto view = m_registry.view<ecs::CWindow>();
		for (auto entity : view) {
			auto &windowComp = view.get<ecs::CWindow>(entity);

			if (windowComp.window) {
				bool isVisible = windowComp.isVisible;
				if (ImGui::MenuItem(windowComp.name.c_str(), nullptr,
									&isVisible)) {
					setWindowVisible(windowComp.name, isVisible);
				}
			}
		}
		ImGui::EndMenu();
	}
}

std::vector<std::string> MWindow::getMenuWindows() {
	std::vector<std::string> menuWindows;
	auto view = m_registry.view<ecs::CWindow>();
	for (auto entity : view) {
		auto &windowComp = view.get<ecs::CWindow>(entity);
		// The category is no longer stored in ecs::CWindow, so this will
		// always return empty This function needs to be updated to reflect the
		// new ecs::CWindow structure For now, returning an empty vector as a
		// placeholder
		menuWindows.push_back(windowComp.name);
	}
	return menuWindows;
}

// Workspace management methods
bool MWindow::loadWorkspace(const std::string &workspaceName) {
	spdlog::info("[Workspace] Requested to load: '{}'", workspaceName);
	// Check if workspace is already loaded in memory
	if (m_workspaces.find(workspaceName) == m_workspaces.end()) {
		spdlog::info(
			"[Workspace] Not in memory, attempting to load from disk...");
		if (!loadWorkspaceConfig(workspaceName)) {
			spdlog::error(
				"[Workspace] Could not find or load workspace file for '{}'",
				workspaceName);
			return false;
		}
	} else {
		spdlog::info("[Workspace] Found in memory, using cached config.");
	}
	const auto &config = m_workspaces[workspaceName];
	spdlog::info("[Workspace] Loaded config: name='{}', description='{}'",
				 config.name, config.description);
	spdlog::info("[Workspace] Window visibility keys:");
	for (const auto &[k, v] : config.windowVisibility)
		spdlog::info(" '{}'", k);
	spdlog::info("");
	// Set all windows to visible by default, then apply config
	std::vector<std::string> allWindows = getAllWindowNames();
	std::set<std::string> allWindowSet(allWindows.begin(), allWindows.end());
	for (const auto &windowName : allWindows) {
		setWindowVisible(windowName, false);
	}
	std::set<std::string> missingWindows;
	// Show only those listed as true in the config
	for (const auto &[windowName, isVisible] : config.windowVisibility) {
		if (allWindowSet.count(windowName)) {
			setWindowVisible(windowName, isVisible);
			spdlog::info("[Workspace] Set '{}' visible={}", windowName,
						 isVisible);
		} else {
			spdlog::warn("[Workspace] Could not find window named '{}' "
						 "referenced in workspace. Skipping.",
						 windowName);
			missingWindows.insert(windowName);
		}
	}
	if (!config.imguiLayout.empty()) {
		spdlog::info("[Workspace] ImGui layout present ({}) bytes.",
					 config.imguiLayout.size());
		// loadImGuiLayout(config.imguiLayout); // Enable if needed
		spdlog::info("[Workspace] ImGui layout loading temporarily disabled "
					 "for debugging");
	} else {
		spdlog::info(
			"[Workspace] No ImGui layout found in workspace, using default");
	}
	m_currentWorkspace = workspaceName;
	spdlog::info("[Workspace] Loaded workspace: '{}'", workspaceName);
	return true;
}

bool MWindow::saveWorkspace(const std::string &workspaceName) {
	WorkspaceConfig currentState = captureCurrentUIState(workspaceName);
	m_workspaces[workspaceName] = currentState;
	if (!saveWorkspaceConfig(workspaceName)) {
		spdlog::error("Failed to save workspace config for '{}'",
					  workspaceName);
		return false;
	}
	spdlog::info("Saved workspace: {}", workspaceName);
	return true;
}

bool MWindow::saveWorkspaceAs(const std::string &workspaceName) {
	WorkspaceConfig newConfig = captureCurrentUIState(workspaceName);
	m_workspaces[workspaceName] = newConfig;
	return saveWorkspace(workspaceName);
}

WorkspaceConfig
MWindow::captureCurrentUIState(const std::string &workspaceName) {
	spdlog::info("Capturing current UI state for workspace: {}", workspaceName);
	WorkspaceConfig config;
	config.name = workspaceName;
	config.description =
		"Custom workspace created on " + std::to_string(std::time(nullptr));
	std::vector<std::string> allWindows = getAllWindowNames();
	spdlog::info("Found {} windows:", allWindows.size());
	for (const auto &windowName : allWindows) {
		bool isVisible = isWindowVisible(windowName);
		if (!isVisible) {
			config.windowVisibility[windowName] = false;
			spdlog::info("  - {} : hidden (captured)", windowName);
		} else {
			spdlog::info("  - {} : visible (not captured)", windowName);
		}
	}
	config.imguiLayout = getCurrentImGuiLayout();
	spdlog::info("ImGui layout captured: {}",
				 config.imguiLayout.empty() ? "empty" : "non-empty");
	spdlog::info("UI state capture complete");
	return config;
}

WorkspaceConfig MWindow::createWorkspaceFromCurrentState(
	const std::string &workspaceName) {
	return captureCurrentUIState(workspaceName);
}

bool MWindow::createWorkspace(const std::string &workspaceName,
									const WorkspaceConfig &config) {
	if (m_workspaces.find(workspaceName) != m_workspaces.end()) {
		spdlog::error("Workspace '{}' already exists", workspaceName);
		return false;
	}
	m_workspaces[workspaceName] = config;
	return saveWorkspaceConfig(workspaceName);
}

bool MWindow::deleteWorkspace(const std::string &workspaceName) {
	if (m_workspaces.find(workspaceName) == m_workspaces.end()) {
		return false;
	}
	std::filesystem::remove(getWorkspaceConfigPath(workspaceName));
	m_workspaces.erase(workspaceName);
	return true;
}

std::vector<std::string> MWindow::getAvailableWorkspaces() const {
	std::vector<std::string> workspaces;
	for (const auto &[name, config] : m_workspaces) {
		workspaces.push_back(name);
	}
	return workspaces;
}

std::vector<std::pair<std::string, std::string>>
MWindow::getAvailableWorkspacesWithNames() const {
	std::vector<std::pair<std::string, std::string>> workspaces;
	for (const auto &[name, config] : m_workspaces) {
		workspaces.push_back({name, config.name});
	}
	return workspaces;
}

WorkspaceConfig
MWindow::getWorkspaceConfig(const std::string &workspaceName) const {
	auto it = m_workspaces.find(workspaceName);
	if (it != m_workspaces.end()) {
		return it->second;
	}
	return WorkspaceConfig{};
}

void MWindow::setWindowVisibility(const std::string &windowName,
										bool visible) {
	if (m_currentWorkspace.empty())
		return;
	auto &config = m_workspaces[m_currentWorkspace];
	config.windowVisibility[windowName] = visible;
}

bool MWindow::getWindowVisibility(const std::string &windowName) const {
	if (m_currentWorkspace.empty())
		return true;
	auto it = m_workspaces.find(m_currentWorkspace);
	if (it == m_workspaces.end())
		return true;
	auto visibilityIt = it->second.windowVisibility.find(windowName);
	return visibilityIt != it->second.windowVisibility.end()
			   ? visibilityIt->second
			   : true;
}

void MWindow::saveCurrentImGuiLayout() {
	ImGui::SaveIniSettingsToDisk(m_mainIniPath.c_str());
}

void MWindow::loadImGuiLayout(const std::string &layoutData) {
	if (layoutData.empty())
		return;
	std::string tempIniPath = m_mainIniPath + ".temp";
	std::ofstream tempFile(tempIniPath);
	if (tempFile.is_open()) {
		tempFile << layoutData;
		tempFile.close();
		ImGui::LoadIniSettingsFromDisk(tempIniPath.c_str());
		std::filesystem::remove(tempIniPath);
	}
}

std::string MWindow::getCurrentImGuiLayout() const {
	std::ifstream file(m_mainIniPath);
	if (file.is_open()) {
		std::stringstream buffer;
		buffer << file.rdbuf();
		return buffer.str();
	}
	return "";
}

void MWindow::ensureWorkspaceDirectory() {
	if (!std::filesystem::exists(m_workspaceDir)) {
		std::filesystem::create_directories(m_workspaceDir);
	}
}

void MWindow::createDefaultWorkspaces() {
	spdlog::info("Workspaces are loaded dynamically from JSON files");
}

void MWindow::loadExistingWorkspaces() {
	spdlog::debug("[DEBUG] Looking for workspaces in: {}",
				  std::filesystem::absolute(m_workspaceDir).string());
	try {
		if (!std::filesystem::exists(m_workspaceDir)) {
			spdlog::debug("[DEBUG] Workspace directory does not exist: {}",
						  m_workspaceDir);
			return;
		}
		for (const auto &entry :
			 std::filesystem::directory_iterator(m_workspaceDir)) {
			spdlog::debug("[DEBUG] Found file: {}", entry.path().string());
			if (entry.is_regular_file() &&
				entry.path().extension() == ".json") {
				std::string workspaceName = entry.path().stem().string();
				spdlog::info("Found workspace file: {}", workspaceName);
				if (loadWorkspaceConfig(workspaceName)) {
					spdlog::info("Successfully loaded workspace: {}",
								 workspaceName);
				} else {
					spdlog::warn("Failed to load workspace: {}", workspaceName);
				}
			}
		}
	} catch (const std::exception &e) {
		spdlog::error("Exception in loadExistingWorkspaces: {}", e.what());
	}
	spdlog::info("Finished loading existing workspaces");
}

std::string
MWindow::getWorkspaceConfigPath(const std::string &workspaceName) const {
	return m_workspaceDir + "/" + workspaceName + ".json";
}

bool MWindow::loadWorkspaceConfig(const std::string &workspaceName) {
	std::string configPath = getWorkspaceConfigPath(workspaceName);
	spdlog::info("[WorkspaceConfig] Attempting to load: {}", configPath);
	if (!std::filesystem::exists(configPath)) {
		spdlog::error("[WorkspaceConfig] File does not exist: {}", configPath);
		return false;
	}
	try {
		std::ifstream file(configPath);
		blot::json j;
		file >> j;
		spdlog::info("[WorkspaceConfig] JSON keys:");
		for (auto it = j.begin(); it != j.end(); ++it)
			spdlog::info(" '{}'", it.key());
		spdlog::info("");
		WorkspaceConfig &config = m_workspaces[workspaceName];
		config.name = j.value("name", workspaceName);
		config.description = j.value("description", "");
		config.windowVisibility.clear();
		if (j.contains("windowVisibility")) {
			for (const auto &[window, visible] :
				 j["windowVisibility"].items()) {
				config.windowVisibility[window] = visible.get<bool>();
			}
		}
		config.imguiLayout = j.value("imguiLayout", "");
		spdlog::info("[WorkspaceConfig] Loaded: name='{}', description='{}', "
					 "windowVisibility={} windows, imguiLayout={}",
					 config.name, config.description,
					 config.windowVisibility.size(),
					 (config.imguiLayout.empty() ? "empty" : "present"));
		return true;
	} catch (const std::exception &e) {
		spdlog::error("[WorkspaceConfig] Error loading workspace config: {}",
					  e.what());
		return false;
	}
}

bool MWindow::saveWorkspaceConfig(const std::string &workspaceName) {
	auto it = m_workspaces.find(workspaceName);
	if (it == m_workspaces.end())
		return false;
	const WorkspaceConfig &config = it->second;
	blot::json j;
	j["name"] = config.name;
	j["description"] = config.description;
	j["windowVisibility"] = config.windowVisibility;
	j["imguiLayout"] = config.imguiLayout;
	std::string configPath = getWorkspaceConfigPath(workspaceName);
	std::ofstream file(configPath);
	if (file.is_open()) {
		file << j.dump(2);
		return true;
	}
	return false;
}

void MWindow::updateMainIniFile() {
	// This method can be called to ensure the main .ini file is up to date
	// It's called automatically when loading workspaces
}

blot::json MWindow::getSettings() const {
	blot::json j;
	// TODO: Serialize window manager state
	return j;
}
void MWindow::setSettings(const blot::json &settings) {
	// TODO: Deserialize window manager state
}

} // namespace blot
