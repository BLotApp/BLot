#include "core/core.h"
#include <memory>
#include <iostream>
#include "app.h"
#include "app_entry.h"

int main(int argc, char* argv[]) {
    auto appInstance = createApp();
    BlotEngine engine(std::move(appInstance));
    engine.run();
    return 0;
} 