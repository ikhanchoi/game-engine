#pragma once
#include <functional>

struct Task {
	std::function<void()> action;
	int priority = 0;
	size_t dependencyMask = 0;
	std::string name;
};
