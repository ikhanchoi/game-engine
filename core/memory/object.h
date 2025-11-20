#pragma once

class Object {
public:
	virtual ~Object() = 0;
};

inline Object::~Object() = default;