#pragma once
#include "core/memory/object.h"
#include "entity_base.h"

class Entity final : public EntityBase, public Object {
public:
	explicit Entity(uint32_t id) : Object(id) {}
};