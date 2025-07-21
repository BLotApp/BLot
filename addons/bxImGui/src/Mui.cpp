#include <algorithm>
#include <filesystem>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <set>
#include <spdlog/spdlog.h>
#include "../assets/fonts/fontRobotoRegular.h"
#include "../third_party/IconFontCppHeaders/IconsFontAwesome5.h"
#include "core/BlotEngine.h"
#include "core/ISettings.h"
#include "core/json.h"
#include "ImGuiRenderer.h"
#include "MWindow.h"
#include "U_ui.h"
#include "ui/windows/AddonManagerWindow.h"
#include "ui/windows/CanvasWindow.h"
#include "ui/windows/CodeEditorWindow.h"
#include "ui/windows/InfoWindow.h"
#include "ui/windows/LogWindow.h"
#include "ui/windows/MainMenuBar.h"
#include "ui/windows/NodeEditorWindow.h"
#include "ui/windows/PropertiesWindow.h"
#include "ui/windows/SaveWorkspaceDialog.h"
#include "ui/windows/StrokeWindow.h"
#include "ui/windows/TextureViewerWindow.h"
#include "ui/windows/ThemeEditorWindow.h"
#include "ui/windows/ThemePanel.h"
#include "ui/windows/ToolbarWindow.h"
#include "ui/windows/WindowManagerPanel.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace blot {
// Helper for icon and color
static const char *getIconForType(NotificationType type) {
	switch (type) {
	case NotificationType::Info:
		return ICON_FA_INFO_CIRCLE;
	case NotificationType::Success:
		return ICON_FA_CHECK_CIRCLE;
	case NotificationType::Warning:
		return ICON_FA_EXCLAMATION_TRIANGLE;
	case NotificationType::Error:
		return ICON_FA_TIMES_CIRCLE;
	default:
		return ICON_FA_INFO_CIRCLE;
	}
}
static ImVec4 getColorForType(NotificationType type) {
	switch (type) {
	case NotificationType::Info:
		return ImVec4(0.2f, 0.6f, 1.0f, 1.0f);
	case NotificationType::Success:
		return ImVec4(0.2f, 0.8f, 0.2f, 1.0f);
	case NotificationType::Warning:
		return ImVec4(1.0f, 0.7f, 0.2f, 1.0f);
	case NotificationType::Error:
		return ImVec4(1.0f, 0.2f, 0.2f, 1.0f);
	default:
		return ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
	}
}

Mui::Mui(GLFWwindow *window) : m_window(window) {
	// Create window manager
	m_windowManager = std::make_unique<MWindow>();

	// Remove WorkspaceManager construction and setup
	m_currentTheme = ImGuiTheme::Light;

	configureWindowSettings();
	// Register TAB shortcut for toggling window visibility
	m_shortcutManager.registerShortcut(
		ImGuiKey_Tab, 0, [this]() { m_bHideWindows = !m_bHideWindows; },
		"Toggle all windows (except menubar)");
}

Mui::~Mui() { shutdownImGui(); }

void Mui::init() { initImGui(); }

void Mui::initImGui() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	// Set up ImGui style (Light theme)
	ImGui::StyleColorsLight();
	ImGuiStyle &style = ImGui::GetStyle();
	// Optionally scale UI by monitor DPI (like ofxBapp)
	float uiScale = 1.0f;
#ifdef _WIN32
	// Windows: use GetDpiForSystem or GetDpiForMonitor if available
	HDC screen = GetDC(0);
	int dpi = GetDeviceCaps(screen, LOGPIXELSX);
	ReleaseDC(0, screen);
	uiScale = dpi / 96.0f;
#endif
	style.ScaleAllSizes(uiScale);

	// Load Roboto font from memory
	ImFontConfig font_cfg;
	font_cfg.FontDataOwnedByAtlas = false;
	io.Fonts->AddFontFromMemoryTTF(fontRobotoRegular, sizeof(fontRobotoRegular),
								   16.0f * uiScale, &font_cfg);
	// Optionally set as default font
	io.FontDefault = io.Fonts->Fonts.back();

	// Load FontAwesome Solid font and merge
	float baseFontSize = 16.0f * uiScale;
	float iconFontSize = baseFontSize * 2.0f / 3.0f;
	static const ImWchar icons_ranges[] = {ICON_MIN_FA, ICON_MAX_16_FA, 0};
	ImFontConfig icons_config;
	icons_config.MergeMode = true;
	icons_config.PixelSnapH = true;
	icons_config.GlyphMinAdvanceX = iconFontSize;
	io.Fonts->AddFontFromFileTTF("assets/fonts/fa-solid-900.ttf", iconFontSize,
								 &icons_config, icons_ranges);

	// Initialize ImGui with GLFW and OpenGL
	ImGui_ImplGlfw_InitForOpenGL(m_window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	// Initialize enhanced text renderer
	m_imguiRenderer = std::make_unique<ImGuiRenderer>();
}

void Mui::shutdown() { shutdownImGui(); }

void Mui::shutdownImGui() {
	// Shutdown ImGui implementation
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Mui::update() {
	spdlog::debug("[Mui] update() called");
	// Start ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Load workspace on first frame only
	static bool workspaceLoaded = false;
	if (!workspaceLoaded) {
		loadWorkspace("current");
		workspaceLoaded = true;
	}

	// Simple dockspace setup
	setupDockspace();

	// Show debug menu bar
	if (m_blotEngine->getDebugMode()) {
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("Debug")) {
				if (ImGui::MenuItem("Exit Debug Mode")) {
					m_blotEngine->setDebugMode(false);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	// --- Shortcuts ---
	m_shortcutManager.processShortcuts();
	m_shortcutManager.showHelpOverlay();
	// -----------------

	// Update UI components
	if (m_windowManager) {
		m_windowManager->update();
	}

	// Render all windows
	renderAllWindows();

	// Handle debug mode toggle with F12 key (using GLFW directly)
	static bool f12Pressed = false;
	if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_F12) == GLFW_PRESS) {
		if (!f12Pressed) {
			m_blotEngine->setDebugMode(!m_blotEngine->getDebugMode());
			f12Pressed = true;
		}
	} else {
		f12Pressed = false;
	}

	// Render ImGui frame
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	// Update and render additional viewports
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
		GLFWwindow *backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

void Mui::handleInput() {
	// Handle global input if needed
	if (m_windowManager) {
		m_windowManager->handleInput();
	}
}

// -----------------------------------------------------------------------------
// Iui interface: render() – handled implicitly in update (already renders)
// -----------------------------------------------------------------------------

void Mui::render() {
    // Rendering is integrated inside update(); nothing required here for now.
}

void Mui::setupDockspace() {
	ImGuiWindowFlags window_flags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	ImGuiViewport *viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	window_flags |=
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

	ImGui::Begin("DockSpace", nullptr, window_flags);

	// Render the main menu bar inside the dockspace window
	if (ImGui::BeginMenuBar()) {
		if (m_mainMenuBar && !m_bHideMainMenuBar) {
			m_mainMenuBar->render();
		}
		ImGui::EndMenuBar();
	}

	ImGui::PopStyleVar(2);
	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f),
					 ImGuiDockNodeFlags_PassthruCentralNode);

	ImGui::End();
}

void Mui::renderAllWindows() {

	// Render all windows
	if (m_windowManager) {
		if (m_bHideWindows) {
			for (const auto &name : m_windowManager->getAllWindowNames()) {
				m_windowManager->setWindowVisible(name, false);
			}
		}
		m_windowManager->renderAllWindows();
	}
	// Render notifications (toasts)
	float y = 20.0f;
	float x = ImGui::GetIO().DisplaySize.x - 350.0f;
	for (auto &n : m_notifications) {
		ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always);
		ImGui::SetNextWindowBgAlpha(0.85f);
		ImGui::PushStyleColor(ImGuiCol_Text, getColorForType(n.type));
		ImGui::Begin(
			(std::string("##toast") + std::to_string(&n - &m_notifications[0]))
				.c_str(),
			nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoFocusOnAppearing |
				ImGuiWindowFlags_NoInputs);
		ImGui::Text("%s  %s", getIconForType(n.type), n.message.c_str());
		ImGui::End();
		ImGui::PopStyleColor();
		y += 48.0f;
		n.timeRemaining -= ImGui::GetIO().DeltaTime;
	}
	m_notifications.erase(std::remove_if(m_notifications.begin(),
										 m_notifications.end(),
										 [](const Notification &n) {
											 return n.timeRemaining <= 0;
										 }),
						  m_notifications.end());
	// Before loading workspace, print all registered window names
	auto allWindows = m_windowManager->getAllWindowNames();
	spdlog::info("Registered windows: {}", allWindows.size());
	for (const auto &w : allWindows)
		spdlog::info("{}", w);
	spdlog::info("");
	// Render modals (blocking popups)
	if (!m_modals.empty()) {
		Modal &m = m_modals.front();
		if (m.open)
			ImGui::OpenPopup(m.title.c_str());
		ImVec2 displaySize = ImGui::GetIO().DisplaySize;
		float modalWidth = (std::min)(600.0f, displaySize.x * 0.8f);
		float modalHeight = (std::min)(300.0f, displaySize.y * 0.6f);
		ImGui::SetNextWindowSize(ImVec2(modalWidth, modalHeight),
								 ImGuiCond_Appearing);
		if (ImGui::BeginPopupModal(m.title.c_str(), nullptr,
								   ImGuiWindowFlags_NoScrollbar)) {
			ImGui::TextColored(getColorForType(m.type), "%s",
							   getIconForType(m.type));
			ImGui::SameLine();
			// Scrollable message area, fixed height
			float buttonAreaHeight = 50.0f;
			ImVec2 childSize =
				ImVec2(0, ImGui::GetWindowHeight() - buttonAreaHeight -
							  ImGui::GetCursorPosY() - 16.0f);
			ImGui::BeginChild("##ModalMessage", childSize, true,
							  ImGuiWindowFlags_AlwaysVerticalScrollbar);
			ImGui::TextWrapped("%s", m.message.c_str());
			ImGui::EndChild();
			ImGui::Spacing();
			// Buttons at bottom right (Cancel left, OK right)
			float buttonWidth = 90.0f;
			float spacing = 12.0f;
			float totalButtonWidth = buttonWidth * 2 + spacing;
			float startX =
				ImGui::GetWindowContentRegionMax().x - totalButtonWidth;
			ImGui::SetCursorPosX(startX > 0 ? startX : 0);
			bool cancel = ImGui::Button("Cancel", ImVec2(buttonWidth, 0));
			ImGui::SameLine();
			bool ok = ImGui::Button("OK", ImVec2(buttonWidth, 0));
			if (ok) {
				if (m.onOk)
					m.onOk();
				m.open = false;
				ImGui::CloseCurrentPopup();
				m_modals.pop_front();
			} else if (cancel) {
				m.open = false;
				ImGui::CloseCurrentPopup();
				m_modals.pop_front();
			}
			ImGui::EndPopup();
		}
	}
}

void Mui::setupWindows(BlotEngine *app) {
	if (!m_windowManager)
		return;

	// Create and register texture viewer window
	auto textureViewerWindow = std::make_shared<TextureViewerWindow>(
		"Texture###MainTexture",
		Window::Flags::NoScrollbar | Window::Flags::NoCollapse);
	m_windowManager->createWindow(textureViewerWindow->getTitle(),
								  textureViewerWindow);

	// Create toolbar window with appropriate flags
	auto toolbarWindow = std::make_shared<ToolbarWindow>(
		"Toolbar###MainToolbar", Window::Flags::NoTitleBar |
									 Window::Flags::NoResize |
									 Window::Flags::NoCollapse);

	m_windowManager->createWindow(toolbarWindow->getTitle(), toolbarWindow);

	// Connect theme panel to toolbar window
	if (m_windowManager->getWindow("ThemePanel")) { // Assuming ThemePanel is
													// registered with MWindow
		auto themePanel = std::dynamic_pointer_cast<ThemePanel>(
			m_windowManager->getWindow("ThemePanel"));
		if (themePanel) {
			themePanel->setToolbarWindow(toolbarWindow);
		}
	}

	// Create canvas window
	auto canvasWindow = std::make_shared<CanvasWindow>(
		"Canvas###MainCanvas",
		Window::Flags::NoScrollbar | Window::Flags::NoCollapse);
	m_windowManager->createWindow(canvasWindow->getTitle(), canvasWindow);

	// Create info window
	auto infoWindow = std::make_shared<InfoWindow>();
	m_windowManager->createWindow(infoWindow->getTitle(), infoWindow);

	// Create properties window
	auto propertiesWindow = std::make_shared<PropertiesWindow>(
		"Properties###MainProperties", Window::Flags::None);
	m_windowManager->createWindow(propertiesWindow->getTitle(),
								  propertiesWindow);

	// Create code editor window
	auto codeEditorWindow = std::make_shared<CodeEditorWindow>(
		"Code Editor###MainCodeEditor", Window::Flags::None);
	m_windowManager->createWindow(codeEditorWindow->getTitle(),
								  codeEditorWindow);

	// Create main menu bar (standalone, not managed by MWindow)
	m_mainMenuBar = std::make_unique<MainMenuBar>("Main Menu Bar");

	// Create addon manager window
	auto addonManagerWindow = std::make_shared<AddonManagerWindow>(
		"Addon Manager###MAddon", Window::Flags::None);
	// Set the central MAddon pointer
	if (m_blotEngine->getAddonManager()) {
		addonManagerWindow->setAddonManager(m_blotEngine->getAddonManager());
	}
	m_windowManager->createWindow(addonManagerWindow->getTitle(),
								  addonManagerWindow);

	// Create node editor window
	auto nodeEditorWindow = std::make_shared<NodeEditorWindow>(
		"Node Editor###NodeEditor", Window::Flags::None);
	m_windowManager->createWindow(nodeEditorWindow->getTitle(),
								  nodeEditorWindow);

	// Create stroke window
	auto strokeWindow = std::make_shared<StrokeWindow>("Stroke###StrokeWindow",
													   Window::Flags::None);
	m_windowManager->createWindow(strokeWindow->getTitle(), strokeWindow);

	// Create and register theme editor window
	auto themeEditorWindow = std::make_shared<ThemeEditorWindow>(
		"Theme Editor###ThemeEditor", Window::Flags::None);
	themeEditorWindow->setUIManager(this);
	m_windowManager->createWindow(themeEditorWindow->getTitle(),
								  themeEditorWindow);

	// Create and register log window
	auto logWindow = std::make_shared<blot::LogWindow>("Log###LogWindow",
													   Window::Flags::None);
	m_windowManager->createWindow(logWindow->getTitle(), logWindow);
	logWindow->setupSpdlogSink();

	// Initialize save workspace dialog before registering
	m_saveWorkspaceDialog = std::make_unique<SaveWorkspaceDialog>(
		"Save Workspace", Window::Flags::Modal);
	// Register save workspace dialog (not shown by default)
	auto saveWorkspaceDialog = std::shared_ptr<SaveWorkspaceDialog>(
		m_saveWorkspaceDialog.get(), [](SaveWorkspaceDialog *) {});
	m_windowManager->createWindow(saveWorkspaceDialog->getTitle(),
								  saveWorkspaceDialog);

	// Register Window Manager panel
	auto windowManagerPanel = std::make_shared<WindowManagerPanel>(
		"Window Manager", m_windowManager.get(), Window::Flags::None);
	m_windowManager->createWindow(windowManagerPanel->getTitle(),
								  windowManagerPanel);
}

void Mui::configureWindowSettings() {
	if (!m_windowManager)
		return;

	// Remove all usages of CWindowSettings and setWindowSettings
}

void Mui::setImGuiTheme(ImGuiTheme theme) {
	ImGuiStyle &style = ImGui::GetStyle();
	switch (theme) {
	case ImGuiTheme::Dark:
		ImGui::StyleColorsDark();
		break;
	case ImGuiTheme::Light:
		ImGui::StyleColorsLight();
		break;
	case ImGuiTheme::Classic:
		ImGui::StyleColorsClassic();
		break;
	case ImGuiTheme::Corporate: {
		ImGui::StyleColorsDark();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] =
			ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] =
			ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] =
			ImVec4(0.28f, 0.34f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] =
			ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] =
			ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] =
			ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
		break;
	}
	case ImGuiTheme::Dracula: {
		ImGui::StyleColorsDark();
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.07f, 0.10f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_HeaderHovered] =
			ImVec4(0.50f, 0.28f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_HeaderActive] =
			ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
		style.Colors[ImGuiCol_ButtonHovered] =
			ImVec4(0.50f, 0.28f, 0.92f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] =
			ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.15f, 0.30f, 1.00f);
		style.Colors[ImGuiCol_FrameBgHovered] =
			ImVec4(0.30f, 0.18f, 0.52f, 1.00f);
		style.Colors[ImGuiCol_FrameBgActive] =
			ImVec4(0.40f, 0.23f, 0.72f, 1.00f);
		break;
	}
	}
	m_currentTheme = theme;
}

void Mui::saveCurrentTheme(const std::string &path) {
	try {
		// Create themes directory if it doesn't exist
		std::filesystem::create_directories("themes");

		// Save current ImGui style to JSON
		blot::json themeJson;
		ImGuiStyle &style = ImGui::GetStyle();

		// Save colors
		blot::json colors;
		for (int i = 0; i < ImGuiCol_COUNT; i++) {
			ImVec4 &color = style.Colors[i];
			colors[std::to_string(i)] = {color.x, color.y, color.z, color.w};
		}
		themeJson["colors"] = colors;

		// Save style variables
		themeJson["style"]["Alpha"] = style.Alpha;
		themeJson["style"]["WindowRounding"] = style.WindowRounding;
		themeJson["style"]["FrameRounding"] = style.FrameRounding;
		themeJson["style"]["GrabRounding"] = style.GrabRounding;

		// Write to file
		std::ofstream file(path);
		file << themeJson.dump(4);
		file.close();

		m_lastThemePath = path;
		std::cout << "Theme saved to: " << path << std::endl;
	} catch (const std::exception &e) {
		std::cerr << "Failed to save theme: " << e.what() << std::endl;
	}
}

void Mui::loadTheme(const std::string &path) {
	try {
		std::ifstream file(path);
		if (!file.is_open()) {
			std::cerr << "Failed to open theme file: " << path << std::endl;
			return;
		}

		blot::json themeJson;
		file >> themeJson;
		file.close();

		ImGuiStyle &style = ImGui::GetStyle();

		// Load colors
		if (themeJson.contains("colors")) {
			auto &colors = themeJson["colors"];
			for (int i = 0; i < ImGuiCol_COUNT; i++) {
				if (colors.contains(std::to_string(i))) {
					auto &color = colors[std::to_string(i)];
					style.Colors[i] =
						ImVec4(color[0], color[1], color[2], color[3]);
				}
			}
		}

		// Load style variables
		if (themeJson.contains("style")) {
			auto &styleJson = themeJson["style"];
			if (styleJson.contains("Alpha"))
				style.Alpha = styleJson["Alpha"];
			if (styleJson.contains("WindowRounding"))
				style.WindowRounding = styleJson["WindowRounding"];
			if (styleJson.contains("FrameRounding"))
				style.FrameRounding = styleJson["FrameRounding"];
			if (styleJson.contains("GrabRounding"))
				style.GrabRounding = styleJson["GrabRounding"];
		}

		m_lastThemePath = path;
		std::cout << "Theme loaded from: " << path << std::endl;
	} catch (const std::exception &e) {
		std::cerr << "Failed to load theme: " << e.what() << std::endl;
	}
}

void Mui::setWindowVisibility(const std::string &windowName, bool visible) {
	if (m_windowManager) {
		m_windowManager->setWindowVisibility(windowName, visible);
	}

	if (m_windowManager) {
		auto window = m_windowManager->getWindow(windowName);
		if (window) {
			if (visible) {
				window->show();
			} else {
				window->hide();
			}
		}
	}
}

void Mui::setWindowVisibilityAll(bool visible) {
	if (m_windowManager) {
		std::vector<std::string> allWindows =
			m_windowManager->getAllWindowNames();
		for (const auto &windowName : allWindows) {
			setWindowVisibility(windowName, visible);
		}
	}
}

bool Mui::getWindowVisibility(const std::string &windowName) const {
	if (m_windowManager) {
		auto window = m_windowManager->getWindow(windowName);
		if (window) {
			return window->isOpen();
		}
	}
	return true;
}

std::vector<std::string> Mui::getAllWindowNames() const {
	if (m_windowManager) {
		return m_windowManager->getAllWindowNames();
	}
	return {};
}

bool Mui::loadWorkspace(const std::string &workspaceName) {
	std::cout << "[Mui] loadWorkspace called with: " << workspaceName
			  << std::endl;
	if (m_windowManager) {
		std::cout << "Loading workspace: " << workspaceName << std::endl;
		m_windowManager->hideAllWindows(
			{"MainMenuBar"}); // Hide all except MainMenuBar
		bool success = m_windowManager->loadWorkspace(workspaceName);
		if (success) {
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
			std::cout << "Workspace loaded successfully" << std::endl;
		} else {
			std::cout << "Failed to load workspace" << std::endl;
		}
		return success;
	}
	return false;
}

bool Mui::saveWorkspace(const std::string &workspaceName) {
	if (m_windowManager) {
		return m_windowManager->saveWorkspace(workspaceName);
	}
	return false;
}

bool Mui::saveWorkspaceAs(const std::string &workspaceName) {
	if (m_windowManager) {
		return m_windowManager->saveWorkspaceAs(workspaceName);
	}
	return false;
}

std::string Mui::getCurrentWorkspace() const {
	if (m_windowManager) {
		return m_windowManager->getCurrentWorkspace();
	}
	return "default";
}

std::vector<std::pair<std::string, std::string>>
Mui::getAvailableWorkspaces() const {
	if (m_windowManager) {
		return m_windowManager->getAvailableWorkspacesWithNames();
	}
	return {};
}

std::vector<std::string> Mui::getAllWorkspaceNames() const {
	if (m_windowManager) {
		return m_windowManager->getAvailableWorkspaces();
	}
	return {};
}

void Mui::setupWindowCallbacks(BlotEngine *app) {
	if (!m_windowManager || !app)
		return;

	// Get the main menu bar and set up its event system
	auto mainMenuBar = std::dynamic_pointer_cast<MainMenuBar>(
		m_windowManager->getWindow("MainMenuBar"));
	if (mainMenuBar) {
		// The MainMenuBar now uses the event system instead of individual
		// callbacks All actions are registered through the event system in
		// BlotApp::registerUIActions
	}

	// Get the toolbar and set up its callbacks
	auto toolbarWindow = std::dynamic_pointer_cast<ToolbarWindow>(
		m_windowManager->getWindow("Toolbar"));
	if (toolbarWindow) {
		toolbarWindow->setOnStrokeWidthChanged([this](float width) {
			// Get stroke window through MWindow
			auto strokeWindow = std::dynamic_pointer_cast<StrokeWindow>(
				m_windowManager->getWindow("Stroke"));
			if (strokeWindow) {
				strokeWindow->setStrokeWidth(static_cast<double>(width));
			}
		});
	}

	// Get the stroke window and set up its callbacks
	auto strokeWindow = std::dynamic_pointer_cast<StrokeWindow>(
		m_windowManager->getWindow("Stroke"));
	if (strokeWindow) {
		strokeWindow->setStrokeWidthCallback([app](double width) {
			// This will be handled by the app
		});

		strokeWindow->setStrokeCapCallback([app](BLStrokeCap cap) {
			// This will be handled by the app
		});

		strokeWindow->setStrokeJoinCallback([app](BLStrokeJoin join) {
			// This will be handled by the app
		});

		strokeWindow->setMiterLimitCallback([app](double limit) {
			// This will be handled by the app
		});

		strokeWindow->setDashArrayCallback(
			[app](const std::vector<double> &dashes) {
				// This will be handled by the app
			});

		strokeWindow->setDashOffsetCallback([app](double offset) {
			// This will be handled by the app
		});

		strokeWindow->setTransformOrderCallback(
			[app](BLStrokeTransformOrder order) {
				// This will be handled by the app
			});
	}
}

void Mui::saveCurrentImGuiLayout() {
	if (m_windowManager) {
		m_windowManager->saveCurrentImGuiLayout();
	}
}

void Mui::showNotification(const std::string &message, NotificationType type,
						   float duration) {
	m_notifications.push_back({message, type, duration});
}
void Mui::showModal(const std::string &title, const std::string &message,
					NotificationType type, std::function<void()> onOk) {
	m_modals.push_back({title, message, type, onOk, true});
}

blot::json Mui::getSettings() const {
	blot::json j;
	j["theme"] = static_cast<int>(m_currentTheme);
	j["lastThemePath"] = m_lastThemePath;
	// Optionally, save window manager state
	if (m_windowManager) {
		j["windowManager"] = m_windowManager->getSettings();
	}
	// Add more UI state as needed
	return j;
}

void Mui::setSettings(const blot::json &settings) {
	if (settings.contains("theme"))
		m_currentTheme = static_cast<ImGuiTheme>(settings["theme"].get<int>());
	if (settings.contains("lastThemePath"))
		m_lastThemePath = settings["lastThemePath"];
	if (settings.contains("windowManager") && m_windowManager) {
		m_windowManager->setSettings(settings["windowManager"]);
	}
	// Restore more UI state as needed
}

} // namespace blot
