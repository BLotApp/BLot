#include "core/core.h"
#include <memory>
#include "app_entry.h"

int main() {
    auto app = createApp();
    blot::BlotEngine engine(std::move(app));
    engine.run();
    return 0;
} 