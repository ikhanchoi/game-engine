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
	virtual void visit(class Visitor& visitor) = 0;

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

template <typename Concrete>
class Pool : public PoolBase {
static_assert(std::is_base_of_v<Object, Concrete>);
static_assert(!std::is_abstract_v<Concrete>);
	struct Slot {
        Concrete object;
        uint32_t generation = 0;
        bool alive = false;
    };
	std::vector<Slot> slots;
    std::vector<uint32_t> frees;
public:
	explicit Pool() = default;
	~Pool() override = default;

	Handle add(Object* object) override;
	void remove(const Handle& handle) override;
	Object* access(const Handle& handle) override;
	void forEach(const std::function<void(Object*)>& function) override;
	void forEach(const std::function<void(Concrete*)>& function);
};



template <typename T>
struct Tree {
	T data;
    std::string name;
    std::vector<std::shared_ptr<Tree>> children;
	Tree(T data, const std::string& name) : data(data), name(name) {}
};

template <typename T>
std::shared_ptr<Tree<T>> find(const std::shared_ptr<Tree<T>>& node, const T& target) {
    if (node->data == target)
        return node;
    for (const auto& child : node->children)
        if (auto found = find(child, target))
            return found;
    return nullptr;
}



class Manager {
protected:
	uint32_t id;
	std::string name;
	std::shared_ptr<Tree<std::type_index>> root;
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pool; // manager hold pools
    std::unordered_map<std::type_index, std::function<std::unique_ptr<Object>(uint32_t, const std::string&)>> factory;
	std::unordered_map<std::type_index, std::weak_ptr<Manager>> manager; // type is of manager
public:
	explicit Manager(uint32_t id, const std::string& name, std::type_index rootType) : id(id), name(name) {
		root = std::make_shared<Tree<std::type_index>>(rootType, name.substr(0, name.find(" "))); // type is the first word of name
	}
	virtual ~Manager() = default;
	virtual void visit(class Visitor& visitor) = 0;

	void setId(unsigned int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	template <typename ParentType, typename ChildType>
	void registerType();
	template <typename Concrete>
	void registerPool();
	void setManager(std::weak_ptr<Manager> manager);
	unsigned int getId() const { return id; }
	const std::string& getName() const { return name; }
	const std::shared_ptr<Tree<std::type_index>>& getRoot() const { return root; }
	const std::unordered_map<std::type_index, std::shared_ptr<PoolBase>>& getPool() { return pool; }
	std::weak_ptr<Manager> getManager(std::type_index managerType);

	Handle create(const std::type_index& type, const std::string& name);
	void destroy(const Handle& handle);

	template <typename Type>
	Type* access(const Handle& handle);
	template <typename Type, typename Function>
	void forEach(const std::type_index& concrete, Function&& function);
	template <typename Concrete, typename Function>
	void forEach(Function&& function);
};


class WindowManager;
class ResourceManager;
class ComponentManager;
class EntityManager;





/*--------*/
/* System */
/*--------*/

class RenderSystem;






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

	template <typename Module>
	void registerModule();
	template <typename Module>
	std::weak_ptr<Manager> getManager();
	template <typename ManagerType>
	ManagerType* access();
};




/*---------*/
/* Visitor */
/*---------*/

class Visitor {
public:
	virtual void visit(class Window& window) = 0;
	virtual void visit(class WindowManager& windowManager) = 0;

	virtual void visit(class ModelResource& modelResource) = 0;
	virtual void visit(class ShaderResource& shaderResource) = 0;
	virtual void visit(class ResourceManager& resourceManager) = 0;

	virtual void visit(class Entity& entity) = 0;
	virtual void visit(class EntityManager& entityManager) = 0;

	virtual void visit(class RenderComponent& renderComponent) = 0;
	virtual void visit(class TransformComponent& transformComponent) = 0;
	virtual void visit(class CameraComponent& cameraComponent) = 0;
	virtual void visit(class LightComponent& lightComponent) = 0;


};

class EmptyVisitor : public Visitor {
public:
	void visit(Window& window) override {};
	void visit(WindowManager& windowManager) override {};

	void visit(ModelResource& modelResource) override {}
	void visit(ShaderResource& shaderResource) override {}
	void visit(ResourceManager& resourceManager) override {}

	void visit(Entity& entity) override {}
	void visit(EntityManager& entityManager) override {}

	void visit(RenderComponent& renderComponent) override {}
	void visit(TransformComponent& transformComponent) override {}
	void visit(CameraComponent& cameraComponent) override {}
	void visit(LightComponent& lightComponent) override {}
};



#include "core.inl"

}


#endif//GAME_ENGINE_CORE_H
