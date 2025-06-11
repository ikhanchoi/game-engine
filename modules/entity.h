#ifndef GAME_ENGINE_ENTITY_H
#define GAME_ENGINE_ENTITY_H

#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "component.h"

namespace ikhanchoi {

class Entity {
protected:
	unsigned int id;// id counted for each type
	std::string name;
	std::type_index type;
	bool active = true;

	std::unordered_map<std::type_index, std::unique_ptr<Component>> component;// shared by only one entity
	std::vector<std::shared_ptr<Entity>> children;
	std::shared_ptr<Entity> parent = nullptr;

public:
	explicit Entity(unsigned int id, std::string name, const std::type_index &type)
		: id(id), name(std::move(name)), type(type) {}
	virtual ~Entity() = default;

	void setId(unsigned int id) { this->id = id; }
	void setName(const std::string &name) { this->name = name; }
	void setType(const std::type_index &type) { this->type = type; }
	void setActive(bool active) { this->active = active; }
	unsigned int getId() const { return id; }
	const std::string &getName() const { return name; }
	const std::type_index &getType() const { return type; }
	bool isActive() const { return active; }

	virtual void update() = 0;
	virtual void show() = 0;

	void setComponent();
};


class EntityManager {
private:
	std::vector<std::unique_ptr<Entity>> entities; // top entities
public:



};



}

#endif//GAME_ENGINE_ENTITY_H
