#pragma once

#include <sstream>
#include <string>
#include <vector>
#include "imgui.h"

namespace blot {

// ImVec2 operator overloads for vector math
inline ImVec2 operator+(const ImVec2 &a, const ImVec2 &b) {
	return ImVec2(a.x + b.x, a.y + b.y);
}

inline ImVec2 operator-(const ImVec2 &a, const ImVec2 &b) {
	return ImVec2(a.x - b.x, a.y - b.y);
}

inline ImVec2 operator*(const ImVec2 &a, float s) {
	return ImVec2(a.x * s, a.y * s);
}

inline ImVec2 operator/(const ImVec2 &a, float s) {
	return ImVec2(a.x / s, a.y / s);
}

inline std::vector<std::string> splitCommaSeparated(const std::string &input) {
	std::vector<std::string> result;
	std::stringstream ss(input);
	std::string item;
	while (std::getline(ss, item, ',')) {
		// Trim whitespace
		size_t start = item.find_first_not_of(" \t");
		size_t end = item.find_last_not_of(" \t");
		if (start != std::string::npos && end != std::string::npos)
			result.push_back(item.substr(start, end - start + 1));
		else if (start != std::string::npos)
			result.push_back(item.substr(start));
		// else skip empty/whitespace-only
	}
	return result;
}

} // namespace blot
