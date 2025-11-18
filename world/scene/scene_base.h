#pragma once
#include "core/containers/tree.h"

#include <bitset>


class SceneBase {
	friend class EntityManager;
	friend class ComponentManager;

protected:
	Forest<Handle<Entity>> sceneGraph;
	std::unordered_map<Handle<Entity>, std::bitset<64>> componentMask; // dense
	std::unordered_map<Handle<Entity>, std::unordered_map<std::type_index, Handle<class Component>>> componentMap; // sparse, TODO: type-erased handle?

	std::unordered_map<Handle<Entity>, std::string> entityName; // for editor
	//std::unordered_map<Handle<Entity>, enum Icon(?)> entityIcon; // for editor

public:
	virtual ~SceneBase() = 0;
};

inline SceneBase::~SceneBase() = default;