#include <memory>
#include "app.h"
#include "core/BlotEngine.h"

int main(int, char **) {
	auto app = std::make_unique<ExampleEditorApp>();
	blot::BlotEngine engine(std::move(app));
	engine.run();
	return 0;
}
