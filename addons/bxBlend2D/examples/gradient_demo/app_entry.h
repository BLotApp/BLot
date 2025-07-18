#pragma once
#include <memory>
#include "app.h"
#include "core/IApp.h"

std::unique_ptr<blot::IApp> createApp(); 