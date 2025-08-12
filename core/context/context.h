#ifndef GAME_ENGINE_CONTEXT_H
#define GAME_ENGINE_CONTEXT_H

#include <string>
#include <typeindex>
#include <unordered_map>

#include "core/managers/manager_base.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace ikhanchoi {

class Context {
	std::string name;
	int width, height;
	GLFWwindow* glfwWindow;
	std::unordered_map<std::type_index, std::unique_ptr<ManagerBase>> manager;
public:
	explicit Context(std::string name, int width, int height);
	~Context();

	GLFWwindow* getGlfwWindow() { return glfwWindow; }
	template <typename ManagerType>
	void registerManager();
	ManagerBase* getManager(std::type_index managerType);
	template <typename ManagerType>
	ManagerType* getManager();

};

#include "context.inl"

}
#endif//GAME_ENGINE_CONTEXT_H
