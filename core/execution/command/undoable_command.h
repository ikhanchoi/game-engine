#pragma once
#include "command.h"

class UndoableCommand final : public Command {
	void (*revert)(void*);
	void* revertPayload;
public:
	explicit UndoableCommand(
		void (*perform)(void*), void* performPayload,
		void (*revert)(void*), void* revertPayload)
		: Command(perform, performPayload),
		  revert(revert), revertPayload(revertPayload) {}
	void undo() const { revert(revertPayload); }
	UndoableCommand clone() const {
		return UndoableCommand(perform, payload, revert, revertPayload);
	}
};