#pragma once

#include <imgui.h>
#include <imgui_node_editor.h>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>
#include <algorithm>

// Show the node editor window. Pass a pointer to a bool for open/close state.
void NodeEditor_ShowWindow(bool* p_open); 