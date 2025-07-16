#pragma once
#include <memory>
#include "framework/IWork.h"

std::unique_ptr<IWork> createWork(); 