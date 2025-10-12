#pragma once
#include "commands/command_base.h"
#include "task.h"

#include <queue> // TODO: optimize with custom queue. (lock-free ring buffer, mpmc queue, memory pool, etc.)

class Scheduler {
public:
	explicit Scheduler() = default;
	~Scheduler() = default;

	void submit(const Task& task);
	void execute();
};



class CommandScheduler {
	std::queue<CommandBase> commands;
};

class TaskScheduler {
	std::queue<Task> tasks;
};
