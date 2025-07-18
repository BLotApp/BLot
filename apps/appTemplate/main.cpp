#include "core/core.h"
#include <memory>
#include <iostream>
#include "app.h"

int main(int argc, char* argv[]) {
    auto appInstance = std::make_unique<AppTemplate>();
    blot::BlotEngine engine(std::move(appInstance));
    engine.run();
    return 0;
} 