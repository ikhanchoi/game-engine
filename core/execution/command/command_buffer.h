#pragma once
#include "command.h"

class CommandBuffer {
	std::vector<Command> commands;
	bool executing = false;

public:
	void (*onCommandExecuted)(Command&);

	void submit(std::function<void()> perform) {
		if (executing)
			throw std::runtime_error("Error: (CommandBuffer::push) Cannot push command while executing.");
		commands.emplace_back(std::move(perform));
	}

	void flush() {
		executing = true;
		for (auto& command : commands) {
			command.execute();
			if (onCommandExecuted)
				onCommandExecuted(command);
		}
		commands.clear();
		executing = false;
	}
};
