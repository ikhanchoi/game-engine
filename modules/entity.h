#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "modules/core.h"
#include "modules/component.h"

namespace ikhanchoi {

class Entity : public Object {
	std::unordered_map<std::type_index, std::weak_ptr<Component>> component; // shared by only one entity
	std::vector<std::weak_ptr<Entity>> children;
	std::weak_ptr<Entity> parent;
public:
	explicit Entity(unsigned int id, const std::string& name) : Object(id, name) {}
	~Entity() = default;
	void visit(Visitor& visitor) override { visitor.visit(*this); }
	static std::shared_ptr<Manager> generateManager() {
		return std::move(std::dynamic_pointer_cast<Manager>(std::make_shared<EntityManager>())); }
	static std::string getTypeName() { return "Entity"; }


	template <typename ComponentType>
	void setComponent(std::unique_ptr<ComponentType> component) {
		static_assert(std::is_base_of<Component, ComponentType>::value);
		if (this->component.find(typeid(ComponentType)) != this->component.end()) {
			//this->component[typeid(ComponentType)]->setActive(false);
			//this->component[typeid(T)]->setEntity(nullptr);
		}
		this->component[typeid(ComponentType)] = std::move(component);
	}
	std::unordered_map<std::type_index, std::weak_ptr<Component>>& getComponent() {
		return component;
	}
	template <typename ComponentType>
	std::shared_ptr<ComponentType> getComponent() {
		static_assert(std::is_base_of<Component, ComponentType>::value);

	}


};


class EntityManager : public Manager {
	std::vector<std::unique_ptr<Entity>> entities; // top entities
public:
	explicit EntityManager() : Manager(0, "Entity manager", typeid(Entity)) {}
	~EntityManager() override = default;
	void visit(Visitor& visitor) override { visitor.visit(*this); }

	const std::vector<std::unique_ptr<Entity>>& getEntities() { return entities; }

	std::unique_ptr<Entity>& addEntity(const std::string& name) {
		unsigned int id = entities.size();
		entities.emplace_back(std::make_unique<Entity>(id, name));
		return entities.back();
	}

	void removeEntity(unsigned int id) {
		if (id < entities.size()) {
			entities[id]->setActive(false); // mark as inactive
			entities[id] = nullptr; // remove from the vector
		}
	}

};



}

#endif//GAME_ENGINE_ENTITY_H
