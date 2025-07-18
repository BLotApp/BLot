#pragma once
#include "app.h"
#include <memory>

inline std::unique_ptr<IApp> createApp() {
    return std::make_unique<SampleUiApp>();
} 