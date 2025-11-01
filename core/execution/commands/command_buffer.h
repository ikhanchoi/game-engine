#pragma once
#include "../command_base.h"

class CommandBuffer {
	std::vector<std::unique_ptr<CommandBase>> commands; // this may be gonna be a typed map like event dispatcher.
	bool executing = false;

public:
	std::function<void(CommandBase&)> onCommandExecuted;

	void submit(std::unique_ptr<CommandBase> command) {
		if (executing)
			throw std::runtime_error("Error: (CommandBuffer::push) Cannot push command while executing.");
		commands.push_back(std::move(command));
	}

	void flush() {
		executing = true;
		for (auto& command : commands) {
			command->execute();
			if (onCommandExecuted)
				onCommandExecuted(*command);
		}
		commands.clear();
		executing = false;
	}
};
