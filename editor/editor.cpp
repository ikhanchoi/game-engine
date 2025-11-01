#include "editor.h"
#include "world/scene/scene_manager.h"
#include "world/entity/entity_manager.h"
#include "editor/ui/ui_system.h"
#include "editor/ui/panel/hierarchy/hierarchy_panel.h"
#include "editor/ui/panel/inspector/inspector_panel.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
// #include <thread>

Editor::Editor() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	glfwWindow = glfwCreateWindow(1920, 1080, "Game Engine Editor", nullptr, nullptr);
	if (!glfwWindow) {
		glfwTerminate();
		throw std::runtime_error("Error: (Editor::Editor) Failed to create GLFW window for Editor.");
	}
	glfwMakeContextCurrent(glfwWindow);
	glfwSwapInterval(1); // Enable vsync
	glewInit();

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	initializeEditWorld();
}

Editor::~Editor() {
	terminateEditWorld();
	try {
		glfwDestroyWindow(glfwWindow);
		glfwTerminate();
	} catch (const std::exception &e) {
		throw std::runtime_error("Error: (Editor::~Editor) Failed to clean up editor resources: " + std::string(e.what()));
	}
}


void Editor::loop() {
	while (!glfwWindowShouldClose(glfwWindow)) {
		glfwWaitEventsTimeout(1.0/120.0); // instead of glfwPollEvents for MacOS
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // in render system?

		if (!playWorld) { // edit mode
			editWorld->update();
		}
		else { // play mode

		}

		glfwSwapBuffers(glfwWindow); // in render system?
		// std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}



void Editor::initializeEditWorld() {
	editWorld = std::make_unique<World>();

	editWorld->startup<SceneManager>();
	editWorld->startup<EntityManager>();

	editWorld->startup<UISystem>();
	editWorld->startup<InspectorPanel>(typeid(UISystem));
	editWorld->startup<HierarchyPanel>(typeid(UISystem));



	editWorld->flush(); // listener subscription
}

void Editor::terminateEditWorld() {

	editWorld->shutdown<HierarchyPanel>();
	editWorld->shutdown<InspectorPanel>();
	editWorld->shutdown<UISystem>();

	editWorld->shutdown<EntityManager>();
	editWorld->shutdown<SceneManager>();
	editWorld.reset();
}

World* Editor::getEditWorld() const {
	return editWorld.get();
}

void Editor::initializePlayWorld() {
	playWorld = std::make_unique<World>();
}

void Editor::terminatePlayWorld() {
	playWorld.reset();
}

World* Editor::getPlayWorld() const {
	return playWorld.get();
}