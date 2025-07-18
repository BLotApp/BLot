#pragma once

namespace blot { namespace components {

struct Selection {
    bool isSelected = false;
    bool isMultiSelected = false;
    int selectionIndex = -1;
};

} } // namespace blot::components 