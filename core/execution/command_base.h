#pragma once

class CommandBase {
protected:
	std::function<void()> perform;
	// virtual std::vector<JobDependency> dependencies() const { return {}; }
	// virtual JobTag tag() const { return JobTag::None; }
	// virtual std::vector<ResourceAccess> resourceAccesses() const { return {}; }


	explicit CommandBase(std::function<void()> function) : perform(std::move(function)) {}

public:
	void execute() const { perform(); }
	virtual ~CommandBase() = 0;
};

inline CommandBase::~CommandBase() = default;

