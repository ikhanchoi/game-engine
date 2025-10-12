#pragma once
#include "command_base.h"

class Command final : public CommandBase {
public:
	explicit Command(std::function<void()> function) : CommandBase(std::move(function)) {}
};