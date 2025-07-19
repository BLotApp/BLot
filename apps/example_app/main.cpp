#include <memory>
#include "app.h"
#include "core/BlotEngine.h"

int main(int argc, char *argv[]) {
	auto appInstance = std::make_unique<ExampleApp>();
	blot::BlotEngine engine(std::move(appInstance));
	engine.run();
	return 0;
}
