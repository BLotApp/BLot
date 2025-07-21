#include <memory>
#include "app.h"
#include "core/BlotEngine.h"

int main() {
	auto app = std::make_unique<FileBrowserApp>();
	blot::BlotEngine engine(std::move(app));
	engine.run();
	return 0;
}
