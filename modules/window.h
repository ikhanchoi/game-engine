#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "resource.h"

namespace ikhanchoi {

class Window {
private:
	std::string name;
	int width, height;
	GLFWwindow *window;

	bool showDemoWindow = false;

	ResourceManager *resourceManager;

public:
	explicit Window(std::string name, int width, int height)
		: name(std::move(name)), width(width), height(height) {}

	void setName(const std::string &name) { this->name = name; }
	void setWidth(int width) { this->width = width; }
	void setHeight(int height) { this->height = height; }
	void setResourceManager(ResourceManager *resourceManager) { this->resourceManager = resourceManager; }

	void pollEvents() { glfwPollEvents(); }
	void swapBuffers() { glfwSwapBuffers(window); }
	bool shouldClose() { return glfwWindowShouldClose(window); }

	void initialize();
	void terminate();
	void show();

};

}

#endif//GAME_ENGINE_WINDOW_H
