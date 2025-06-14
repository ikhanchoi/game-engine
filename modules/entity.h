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
	std::unordered_map<std::type_index, std::unique_ptr<Component>> component; // shared by only one entity
	std::vector<Entity*> children;
	Entity* parent = nullptr;
public:
	explicit Entity(unsigned int id, const std::string& name) : Object(id, name) {}
	~Entity() = default;
	void update(Updater& updater) override { updater.update(*this); }


	template <typename ComponentType>
	void setComponent(std::unique_ptr<ComponentType> component) {
		static_assert(std::is_base_of<Component, ComponentType>::value);
		if (this->component.find(typeid(ComponentType)) != this->component.end()) {
			this->component[typeid(ComponentType)]->setActive(false);
			//this->component[typeid(T)]->setEntity(nullptr);
		}
		this->component[typeid(ComponentType)] = std::move(component);
	}
	std::unordered_map<std::type_index, std::unique_ptr<Component>>& getComponent() {
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
	explicit EntityManager(unsigned int id, const std::string& name);
	~EntityManager() override = default;
	void update(Updater& updater) override { updater.update(*this); }

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
