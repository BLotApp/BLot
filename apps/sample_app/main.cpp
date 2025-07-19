#include "app.h"
#include "core/BlotEngine.h"
#include "core/core.h"
#include <iostream>
#include <memory>

int main(int argc, char *argv[]) {
	auto appInstance = std::make_unique<SampleUiApp>();
	blot::BlotEngine engine(std::move(appInstance));
	engine.run();
	return 0;
}
