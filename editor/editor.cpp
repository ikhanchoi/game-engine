#include "editor.h"
#include "editor/systems/ui/ui.h"
#include "game/objects/entity/entity_manager.h"
#include "game/objects/scene/scene_manager.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

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
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if (!playWorld) { // edit mode
			editWorld->update();
		}
		else { // play mode

		}



		glfwSwapBuffers(glfwWindow);




		// subscribe를 맨 처음에 하는 과정에서 ManagerBase에 순수가상함수를 넣어주는 것이 베스트?
		// event들을 모니터할 수 있으면 좋겠다.
		// event 뿐만 아니라 다른 매니저들에 대해서도 전부 모니터링 가능하려나?

	}
}



void Editor::initializeEditWorld() {
	editWorld = std::make_unique<World>();
	editWorld->startup<SceneManager>();
	editWorld->startup<EntityManager>();
	editWorld->startup<UISystem>();


	editWorld->update(); // for listener subscription
}

void Editor::terminateEditWorld() {
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