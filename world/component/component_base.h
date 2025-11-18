#pragma once

class ComponentBase {
public:
	virtual ~ComponentBase() = 0;
};

inline ComponentBase::~ComponentBase() = default;