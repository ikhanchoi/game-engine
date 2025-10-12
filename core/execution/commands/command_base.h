#pragma once

class CommandBase {
protected:
	std::function<void()> perform;

	explicit CommandBase(std::function<void()> function) : perform(std::move(function)) {}

public:
	void execute() const { perform(); }
	virtual ~CommandBase() = 0;
};

inline CommandBase::~CommandBase() = default;

