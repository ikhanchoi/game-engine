#ifndef GAME_ENGINE_CONTEXT_H
#define GAME_ENGINE_CONTEXT_H

#include <string>
#include <typeindex>
#include <unordered_map>

#include "core/execution/manager.h"
#include "core/utils/visitor.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


class Context {
	GLFWwindow* glfwWindow;
	std::unordered_map<std::type_index, std::unique_ptr<ManagerBase>> manager;
	std::unordered_map<std::type_index, std::unique_ptr<VisitorBase>> visitor;
	enum Mode : std::uint8_t { EDIT, PLAY } mode = EDIT;


public:
	explicit Context(const std::string& name, int width, int height);
	~Context();

	template <typename ManagerType>
	ManagerType* registerManager() {
		static_assert(std::is_base_of<ManagerBase, ManagerType>::value, "ManagerType must inherit from ManagerBase.");
		if (manager.find(typeid(ManagerType)) != manager.end())
			throw std::runtime_error("Error: (Context::registerManager) Manager already registered.");
		manager[typeid(ManagerType)] = std::make_unique<ManagerType>(this);
		return static_cast<ManagerType*>(manager[typeid(ManagerType)].get());
	}
	template <typename VisitorType>
	VisitorType* registerVisitor() {
		static_assert(std::is_base_of<VisitorBase, VisitorType>::value, "VisitorType must inherit from VisitorBase.");
		if (visitor.find(typeid(VisitorType)) != visitor.end())
			throw std::runtime_error("Error: (Context::registerVisitor) Visitor already registered.");
		visitor[typeid(VisitorType)] = std::make_unique<VisitorType>(this);
		return static_cast<VisitorType*>(visitor[typeid(VisitorType)].get());
	}

	GLFWwindow* getGlfwWindow() { return glfwWindow; }
	ManagerBase* getManager(std::type_index managerType) { return this->manager[managerType].get(); }
	template <typename ManagerType>
	ManagerType* getManager() {
		static_assert(std::is_base_of<ManagerBase, ManagerType>::value, "ManagerType must inherit from ManagerBase.");
		auto it = manager.find(typeid(ManagerType));
		if (it != manager.end())
			return static_cast<ManagerType*>(it->second.get());
		else
			throw std::runtime_error("Error: (Context::getManager) Manager not found: " + std::string(typeid(ManagerType).name()));
	}
	VisitorBase* getVisitor(std::type_index visitorType) { return this->visitor[visitorType].get(); }

};

#endif//GAME_ENGINE_CONTEXT_H
