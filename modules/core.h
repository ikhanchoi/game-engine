#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include <iostream>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


namespace ikhanchoi {

/*--------*/
/* Object */
/*--------*/

class Object {
protected:
	uint32_t id; // id counted for each type
	std::string name;
	bool active = true;
public:
	explicit Object(uint32_t id, const std::string& name) : id(id), name(name) {}
	virtual ~Object() = default;
	virtual void update(class Updater& updater) = 0;

	void setId(uint32_t id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setActive(bool active) { this->active = active; }
	uint32_t getId() const { return id; }
	const std::string& getName() const { return name; }
	bool isActive() const { return active; }
	bool* accessActive() { return &active; }
};

class Window;

class Resource;
class ModelResource;
class ShaderResource;

class Component;
class RenderComponent;
class TransformComponent;
class CameraComponent;
class LightComponent;

class Entity;


/*---------*/
/* Manager */
/*---------*/


struct Handle {
    uint32_t index;
    uint32_t generation;
	std::type_index type;
    bool operator == (const Handle& other) const {
        return index == other.index && generation == other.generation && type == other.type;
    }
};

class PoolBase {
public:
	virtual ~PoolBase() = default;
	virtual Handle add(Object* untypedObject) = 0;
	virtual void remove(const Handle& handle) = 0;
	virtual Object* access(const Handle& handle) = 0;
	virtual void forEach(const std::function<void(Object*)>& function) = 0;
};

template <typename Type>
class Pool : public PoolBase {
static_assert(std::is_base_of_v<Object, Type>);
	struct Slot {
        Type object;
        uint32_t generation = 0;
        bool alive = false;
    };
	std::vector<Slot> slots;
    std::vector<uint32_t> frees;
public:
	explicit Pool() = default;
	~Pool() override = default;

	Handle add(Object* untypedObject) override;
	void remove(const Handle& handle) override;
	Object* access(const Handle& handle) override;
	void forEach(const std::function<void(Object*)>& function) override;
};

class Manager {
protected:
	uint32_t id;
	std::string name;
	std::unordered_map<std::type_index, std::string> types; // will not be used like types[typeid]
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pool; // manager hold pools
    std::unordered_map<std::type_index, std::function<std::unique_ptr<Object>(uint32_t, const std::string&)>> factory;
	std::unordered_map<std::type_index, std::weak_ptr<Manager>> manager; // type is of manager
public:
	explicit Manager(unsigned int id, const std::string& name) : id(id), name(name) {}
	virtual ~Manager() = default;
	virtual void update(class Updater& updater) = 0;

	void setId(unsigned int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	template <typename Type>
	void registerType(const std::string& typeName);
	void setManager(std::weak_ptr<Manager> manager);
	unsigned int getId() const { return id; }
	const std::string& getName() const { return name; }
	const std::unordered_map<std::type_index, std::string>& getTypes() const { return types; }
	std::weak_ptr<Manager> getManager(std::type_index managerType);

	Handle create(const std::type_index& type, const std::string& name);
	void destroy(const Handle& handle);

	Object* access(const Handle& handle);
	template <typename Module, typename Function>
	void forEach(const std::type_index& type, Function&& function);
};

#include "core.inl"

class WindowManager;
class ResourceManager;
class ComponentManager;
class EntityManager;



/*--------*/
/* System */
/*--------*/

class RenderSystem;




/*---------*/
/* Updater */
/*---------*/

class Updater {
public:
	virtual void update(class Window& window) = 0;
	virtual void update(class WindowManager& windowManager) = 0;

	virtual void update(class ModelResource& modelResource) = 0;
	virtual void update(class ShaderResource& shaderResource) = 0;
	virtual void update(class ResourceManager& resourceManager) = 0;

	virtual void update(class Entity& entity) = 0;
	virtual void update(class EntityManager& entityManager) = 0;

	virtual void update(class RenderComponent& renderComponent) = 0;
	virtual void update(class TransformComponent& transformComponent) = 0;
	virtual void update(class CameraComponent& cameraComponent) = 0;
	virtual void update(class LightComponent& lightComponent) = 0;


};

class EmptyUpdater : public Updater {
public:
	void update(Window& window) override {};
	void update(WindowManager& windowManager) override {};

	void update(ModelResource& modelResource) override {}
	void update(ShaderResource& shaderResource) override {}
	void update(ResourceManager& resourceManager) override {}

	void update(Entity& entity) override {}
	void update(EntityManager& entityManager) override {}

	void update(RenderComponent& renderComponent) override {}
	void update(TransformComponent& transformComponent) override {}
	void update(CameraComponent& cameraComponent) override {}
	void update(LightComponent& lightComponent) override {}
};




/*---------*/
/* Context */
/*---------*/

class Context {
	std::string name;
	int width, height;
	GLFWwindow* glfwWindow;
	std::unordered_map<std::type_index, std::shared_ptr<Manager>> manager;
public:
	explicit Context(std::string name, int width, int height);
	~Context();

	GLFWwindow* getGlfwWindow() { return glfwWindow; }

	template <typename ManagerType>
	std::shared_ptr<ManagerType> registerManager(const std::string& managerName) {
		static_assert(std::is_base_of<Manager, ManagerType>::value);
		if (this->manager.find(typeid(ManagerType)) != this->manager.end())
			throw std::runtime_error("Error: (Context::registerModule) Manager type already registered.");
		auto manager = std::make_shared<ManagerType>(this->manager.size(), managerName);
		this->manager[typeid(ManagerType)] = manager;
		return manager;
	}
};


}


#endif//GAME_ENGINE_CORE_H
