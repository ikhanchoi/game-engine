#pragma once
#include "world.h"
#include <any>
#include <typeindex>

class SystemBase {
	World& world;
	std::unordered_map<std::type_index, std::any> storage;

protected:
	explicit SystemBase(World& world) : world(world) {}

public:

	virtual ~SystemBase() = 0;
};

inline SystemBase::~SystemBase() = default;