#include "app.h"
#include "core/BlotEngine.h"

void AppTemplate::setup() {
	spdlog::info("App Template setup!");
	m_engine->setClearColor(0.7f, 0.45f, 0.9f, 1.0f);
}
