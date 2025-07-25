#ifndef GAME_ENGINE_CORE_H
#define GAME_ENGINE_CORE_H

#include <iostream>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>
#include <vector>
#include <functional>

#include <GL/glew.h>
#include <GLFW/glfw3.h>




namespace ikhanchoi {



/*---------*/
/* Visitor */
/*---------*/

class Visitor {
public:
	virtual void visit(class ManagerWindow& managerWindow) const = 0;

	virtual void visit(class ModelResource& modelResource) const = 0;
	virtual void visit(class ShaderResource& shaderResource) const = 0;

	virtual void visit(class Entity& entity) const = 0;

	virtual void visit(class RenderComponent& renderComponent) const = 0;
	virtual void visit(class TransformComponent& transformComponent) const = 0;
	virtual void visit(class CameraComponent& cameraComponent) const = 0;
	virtual void visit(class LightComponent& lightComponent) const = 0;

};

class EmptyVisitor : public Visitor {
	class Context* context = nullptr;
public:
	void setContext(class Context* context) { this->context = context; }
	Context* getContext() const { return context; }

	void visit(ManagerWindow& managerWindow) const override {};

	void visit(ModelResource& modelResource) const override {}
	void visit(ShaderResource& shaderResource) const override {}

	void visit(Entity& entity) const override {}

	void visit(RenderComponent& renderComponent) const override {}
	void visit(TransformComponent& transformComponent) const override {}
	void visit(CameraComponent& cameraComponent) const override {}
	void visit(LightComponent& lightComponent) const override {}
};


/*--------*/
/* Module */
/*--------*/



template<typename ModuleType>
class CRTPModule {
public:
    static std::unique_ptr<class ManagerBase> generateManager() {
        static_assert(std::is_same_v<decltype(ModuleType::generateManager()), std::unique_ptr<ManagerBase>>);
        return ModuleType::generateManager();
    }
};


/*--------*/
/* Object */
/*--------*/

// An abstract base class for diamond inheritance.
class Base {
public:
	virtual ~Base() noexcept = 0;
};


// An abstract mix-in class for concrete classes.
class Object : public virtual Base {
protected:
	uint32_t id;
	std::string name;
	bool active = true;

	explicit Object(uint32_t id, std::string name) : id(id), name(std::move(name)) {}
public:
	void setId(uint32_t id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setActive(bool active) { this->active = active; }
	uint32_t getId() const { return id; }
	const std::string& getName() const { return name; }
	bool isActive() const { return active; }
	bool* accessActive() { return &active; }

	virtual void accept(const class Visitor& visitor) = 0;
};

// Each concrete class actually inherits the CRTP class `Object` to implement the visit method.
/*
 * The virtual inheritance is used to cast the object to the correct type in run-time.
 * For instance, if a class `ModelResource` inherits from `Object` class and `Resource` class,
 * and if `Resource` class inherits from `Module` class, then in order to cast a pointer
 * `Resource* modelResource` that points to a `ModelResource` object, we can use
 * `dynamic_cast<Object*>(modelResource)` to get a pointer to the base class.
 * This is because `Object` is a virtual base class of `ModelResource`, allowing us to safely
 * down-cast it to the correct type.
 */

/*
 * The Visitor pattern is one technique to achieve run-time polymorphism.
 * The accept method will be implemented within CRTP.
 *
 * It is particularly useful for adding new operations to an existing object hierarchy
 * without modifying the classes within that hierarchy (adhering to the Open/Closed Principle),
 * which prevents scattering functionally similar but type-specific method implementations
 * across various source files.
 *
 * It also enables reference to unique member variables of concrete derived classes from a
 * common interface pointer, which the usual virtual functions cannot provide.
 */

// An abstract CRTP base class that injects the implementation of the visit method to each object
template <typename ObjectType>
class CRTPObject : public Object {
protected:
	explicit CRTPObject(uint32_t id, const std::string& name) : Object(id, name) {}
public:
	void accept(const Visitor& visitor) override { visitor.visit(static_cast<ObjectType&>(*this)); } // (1)
};

/* The CRTPObject class is a CRTP base class that provides the implementation of the visit method.
 * It is used to avoid virtual inheritance and to provide a compile-time polymorphism.
 * The ObjectType is the concrete class that inherits from CRTPObject.
 * The ObjectType should be derived from Object, and it should not be abstract. */

/* (1) The class Visitor should be defined before this method, and forward declaration of Visitor is not sufficient. */






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
	std::type_index type; // object type
	Handle() : index(0), generation(0), type(typeid(void)) {}
	Handle(uint32_t index, uint32_t generation, std::type_index type) : index(index), generation(generation), type(type) {}
};

class PoolBase {
public:
	virtual ~PoolBase() = default;
	virtual Handle add(Object* untypedObject) = 0;
	virtual void remove(const Handle& handle) = 0;
	virtual Object* access(const Handle& handle) = 0;
	virtual void forEach(const std::function<void(Object*)>& function) = 0;
};

template <typename ObjectType>
class Pool : public PoolBase {
static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	struct Slot {
        alignas(ObjectType) char objectBuffer[sizeof(ObjectType)];
        uint32_t generation = 0;
        bool alive = false;
    };
	std::vector<Slot> slots;
    std::vector<uint32_t> frees;
public:
	explicit Pool(size_t initialCapacity = 64) {
        slots.reserve(initialCapacity);
        for (size_t i = 0; i < initialCapacity; ++i) {
            slots.emplace_back();
            frees.push_back(i);
        }
        std::reverse(frees.begin(), frees.end());
    }
	Handle add(Object* untypedObject) override;
	void remove(const Handle& handle) override;
	Object* access(const Handle& handle) override; // It returns `Object*` instead of `ObjectType*` for overriding.
	void forEach(const std::function<void(Object*)>& function) override; // It is for up-casted object access.
	void forEach(const std::function<void(ObjectType*)>& function); // It is for down-casted object access.
};


/*
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
*/

// A base class for managers.
// creates and destroys objects by pools, and provides a method to round over the objects.
class ManagerBase {
protected:
	class Context* context; // for accessing other managers via context
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pool;
    std::unordered_map<std::type_index, std::function<std::unique_ptr<Object>(uint32_t, const std::string&)>> factory;
protected:
	Handle create(const std::type_index& type, const std::string& name); // creator for objects
	void destroy(const Handle& handle); // destroyer for objects
public:
	virtual ~ManagerBase() = default;

	// setters and getters
	void setContext(Context* context) { this->context = context; }
	template <typename ObjectType>
	void registerObjectType();
	Context* getContext() { return context; };
	const std::unordered_map<std::type_index, std::shared_ptr<PoolBase>>& getPools() { return pool; } // for iteration

	template <typename BaseType>
	BaseType* access(const Handle& handle); // accessor for objects without directly getting pool by handle

	// for-each loops
	template <typename BaseType, typename Function>
	void forEach(const std::type_index& objectType, Function&& function);
	template <typename ObjectType, typename Function>
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
	std::unordered_map<std::type_index, std::unique_ptr<ManagerBase>> manager;
public:
	explicit Context(std::string name, int width, int height);
	~Context();

	GLFWwindow* getGlfwWindow() { return glfwWindow; }
	template <typename ModuleType>
	void registerModuleType();
	ManagerBase* getManager(std::type_index managerType);
	template <typename ManagerType>
	ManagerType* getManager();

};





#include "core.inl"

}


#endif//GAME_ENGINE_CORE_H
