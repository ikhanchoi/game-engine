#pragma once
#include "../../../core/execution/commands/command_buffer.h"
#include "game/system_base.h"

class UndoRedoSystem : public SystemBase {
	std::vector<std::unique_ptr<UndoableCommand>> undoStack;
	std::vector<std::unique_ptr<UndoableCommand>> redoStack;

public:
	explicit UndoRedoSystem(World& world);

	void attachToCommandBuffer(CommandBuffer& commandBuffer) {
		commandBuffer.onCommandExecuted = [this](CommandBase& command) {
			auto* undoable = dynamic_cast<UndoableCommand*>(&command);
			undoStack.push_back(undoable->clone());
			redoStack.clear();
		};
	}

	void undo() {
		if (undoStack.empty()) return;
		auto command = std::move(undoStack.back());
		undoStack.pop_back();
		command->undo();
		redoStack.push_back(std::move(command));
	}

	void redo() {
		if (redoStack.empty()) return;
		auto command = std::move(redoStack.back());
		redoStack.pop_back();
		command->execute();
		undoStack.push_back(std::move(command));
	}
};