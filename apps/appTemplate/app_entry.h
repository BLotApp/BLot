#pragma once
#include <memory>
#include "app.h"
#include "core/IApp.h"
 
inline std::unique_ptr<IApp> createApp() {
    return std::make_unique<AppTemplate>();
}
