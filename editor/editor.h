#pragma once
#include "world/world.h"

class Editor {
	class GLFWwindow* glfwWindow;
	std::unique_ptr<World> editWorld;
	std::unique_ptr<World> playWorld;

public:
	explicit Editor();

	void initializeEditWorld();
	void initializePlayWorld();

	World* getEditWorld() const;
	World* getPlayWorld() const;

	void loop();

	void terminatePlayWorld();
	void terminateEditWorld();

	~Editor();
};