#include "context.h"


namespace ikhanchoi {

Context::Context(std::string name, int width, int height)
	: name(std::move(name)), width(width), height(height) {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindow = glfwCreateWindow(width, height, this->name.c_str(), nullptr, nullptr);
	if (glfwWindow == nullptr) {
		std::runtime_error("Error: (Context) Failed to create GLFW window.");
		glfwTerminate();
	}
	glfwMakeContextCurrent(glfwWindow);
	//glewExperimental = GL_TRUE;
	glewInit();
}

Context::~Context() {
	try {
		for (auto& [moduleType, upcastedManager] : this->manager)
			upcastedManager.reset();
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	} catch (const std::exception& e) {
		std::runtime_error("Error: (Context::~Context) Failed to clean up context resources: " + std::string(e.what()));
	}
}

ManagerBase* Context::getManager(std::type_index managerType) {
	if (manager.find(managerType) == manager.end())
		throw std::runtime_error("Error: (Context::getManager) Module not registered." + std::string(managerType.name()));
	return this->manager[managerType].get();
}

}