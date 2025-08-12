#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "modules/component.h"

namespace ikhanchoi {



class EntityBase {
public:
	virtual ~EntityBase() = default;
};


class Entity : public EntityBase, public CRTPObject<Entity> {
	std::unordered_map<std::type_index, Handle> component; // a component is shared by only one entity.
	std::vector<std::unique_ptr<Entity>> children;
	Entity* parent; // nullptr if this is a top entity
public:
	explicit Entity(unsigned int id, const std::string& name) : CRTPObject(id, name), children() {}
	virtual ~Entity() override = default;

	void setComponent(const Handle& component) { this->component[component.type] = component; }
	void setParent(Entity* parent) { this->parent = parent; }
	std::unordered_map<std::type_index, Handle>& getComponents() { return component; }
	template <typename ComponentType>
	Handle getComponent() {
		static_assert(std::is_base_of<ComponentBase, ComponentType>::value);
		auto it = component.find(typeid(ComponentType));
		if (it != component.end()) {
			return it->second;
		} else {
			throw std::runtime_error("Error: (Entity::getComponent) Component not found.");
		}
	}
	std::vector<std::unique_ptr<Entity>>& getChildren() { return children; }
	Entity* getParent() { return parent; }
};


class EntityManager : public ManagerBase {
	std::vector<std::unique_ptr<Entity>> entities; // these are top entities.
	uint32_t globalEntityId = 0; // necessary because entities are not managed by pools.
public:
	explicit EntityManager(Context* context) : ManagerBase(context), entities(), globalEntityId(0) {}
	const std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }

	Entity* addEntity(const std::string& name, Entity* parent);
	void removeEntity(Entity* entity);
	void moveEntity(Entity* entity, Entity* parent);

};



}

#endif//GAME_ENGINE_ENTITY_H
