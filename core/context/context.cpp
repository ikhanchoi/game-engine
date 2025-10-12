#include "context.h"


Context::Context(const std::string& name, int width, int height){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindow = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	if (glfwWindow == nullptr) {
		throw std::runtime_error("Error: (Context) Failed to create GLFW windows.");
		glfwTerminate();
	}
	glfwMakeContextCurrent(glfwWindow);
	//glewExperimental = GL_TRUE;
	glewInit();
}

Context::~Context() {
	try {
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	} catch (const std::exception& e) {
		throw std::runtime_error("Error: (Context::~Context) Failed to clean up context resources: " + std::string(e.what()));
	}
}

