#pragma once

class Object {
protected:
    uint32_t id;

    explicit Object(uint32_t id) : id(id) {}

public:
    uint32_t getId() const { return id; }

	virtual ~Object() = 0;
};

inline Object::~Object() = default;