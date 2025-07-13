#include <iostream>
#include "BlotApp.h"

int main(int argc, char* argv[]) {
	std::cout << "Entering main() function" << std::endl;
	(void)argc;
	(void)argv;
	try {
		std::cout << "About to construct BlotApp" << std::endl;
		BlotApp app;
		std::cout << "After BlotApp construction: m_window=" << app.getWindow() << ", m_running=" << app.isRunning() << std::endl;
		std::cout << "About to call app.run()" << std::endl;
		app.run();
		std::cout << "After app.run()" << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
} 