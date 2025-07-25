#include "app.h"
#include "core/U_core.h"

int main(int argc, char *argv[]) {
	blot::App app;
	app.setApp(std::make_unique<GradientTestApp>());
	return app.run(argc, argv);
}
