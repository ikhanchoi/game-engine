#pragma once

class EntityBase {
public:
	virtual ~EntityBase() = 0;
};

inline EntityBase::~EntityBase() = default;