#pragma once

namespace blot {
namespace ecs {

struct CSelection {
	bool isSelected = false;
	bool isMultiSelected = false;
	int selectionIndex = -1;
};

} // namespace ecs
} // namespace blot
