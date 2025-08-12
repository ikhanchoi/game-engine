#ifndef GAME_ENGINE_OBJECT_BASE_H
#define GAME_ENGINE_OBJECT_BASE_H

#include <string>

#include "core/utils/visitor.h"

namespace ikhanchoi {

// An abstract mix-in class for concrete classes.
class Object {
protected:
	uint32_t id;
	std::string name;
	bool active = true;

	explicit Object(uint32_t id, std::string name) : id(id), name(std::move(name)) {}
public:
	virtual ~Object() = default;
	void setId(uint32_t id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setActive(bool active) { this->active = active; }
	uint32_t getId() const { return id; }
	const std::string& getName() const { return name; }
	bool isActive() const { return active; }
	bool* accessActive() { return &active; }

	virtual void accept(const Visitor& visitor) = 0;
};


template <typename ObjectType>
class CRTPObject : public Object {
protected:
	explicit CRTPObject(uint32_t id, const std::string& name) : Object(id, name) {}
public:
	void accept(const Visitor& visitor) override { visitor.visit(static_cast<ObjectType&>(*this)); } // (1)
};

};


#endif//GAME_ENGINE_OBJECT_BASE_H
