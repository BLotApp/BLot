#pragma once
#include "app.h"
#include <memory>

std::unique_ptr<IApp> createApp(); 