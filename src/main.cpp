#include <iostream>
#include "BlotApp.h"

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;
	try {
		BlotApp app;
		app.run();
	} catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	return 0;
} 