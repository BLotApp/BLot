#include <memory>
#include "core/U_core.h"

int main() {
	auto app = std::make_unique<SimpleConsoleApp>();
	blot::BlotEngine engine(std::move(app));
	engine.run();
	return 0;
}
