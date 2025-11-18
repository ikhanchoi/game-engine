#pragma once
#include <functional>

class Command {
	std::function<void()> perform;
public:
	explicit Command(std::function<void()> function) : perform(std::move(function)) {}
	void execute() const { perform(); }
};