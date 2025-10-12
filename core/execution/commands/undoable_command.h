#pragma once
#include "command_base.h"

class UndoableCommand final : public CommandBase {
	std::function<void()> revert;
public:
	explicit UndoableCommand(std::function<void()> perform, std::function<void()> revert)
		: CommandBase(std::move(perform)), revert(std::move(revert)) {}
	void undo() const { revert(); }
	std::unique_ptr<UndoableCommand> clone() const {
		return std::make_unique<UndoableCommand>(perform, revert);
	}
};