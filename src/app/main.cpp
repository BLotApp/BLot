#include <iostream>
#include "BlotApp.h"
#include <spdlog/spdlog.h>

int main(int argc, char* argv[]) {
	spdlog::info("Entering main() function");
	(void)argc;
	(void)argv;
	try {
		spdlog::info("About to construct BlotApp");
		BlotApp app;
		spdlog::info("After BlotApp construction: m_window=0x{:X}, m_running={}", reinterpret_cast<uintptr_t>(app.getWindow()), app.isRunning());
		spdlog::info("About to call app.run()");
		app.run();
		spdlog::info("After app.run()");
	} catch (const std::exception& e) {
		spdlog::error("Error: {}", e.what());
		return 1;
	}
	return 0;
} 