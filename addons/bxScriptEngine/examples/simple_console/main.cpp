#include "core/core.h"
#include <memory>

int main() {
    auto app = std::make_unique<SimpleConsoleApp>();
    blot::BlotEngine engine(std::move(app));
    engine.run();
    return 0;
} 