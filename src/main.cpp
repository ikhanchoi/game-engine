// Modules
#include "modules/core.h"
#include "modules/resource.h"
#include "modules/event.h"
#include "modules/window.h"
#include "modules/entity.h"

// Systems
#include "systems/render.h"


int main() {

	auto context = std::make_unique<ikhanchoi::Context>("GLTF Viewer", 1280, 960);

	// Module registration
	context->registerModule<ikhanchoi::Window>();
	auto* windowManager = context->access<ikhanchoi::WindowManager>();
	windowManager->registerPool<ikhanchoi::Window>();

	context->registerModule<ikhanchoi::Resource>();
	auto* resourceManager = context->access<ikhanchoi::ResourceManager>();
	resourceManager->registerType<ikhanchoi::Resource, ikhanchoi::ModelResource>();
	resourceManager->registerPool<ikhanchoi::ModelResource>();
	resourceManager->registerType<ikhanchoi::Resource, ikhanchoi::ShaderResource>();
	resourceManager->registerPool<ikhanchoi::ShaderResource>();



	context->registerModule<ikhanchoi::Entity>();
	auto* entityManager = context->access<ikhanchoi::EntityManager>();
	// entityManager->registerPool<ikhanchoi::Entity>();


	// Add objects

	//  there is a default window for window manager
	windowManager->addWindow<ikhanchoi::Window>("Resource manager", context->getManager<ikhanchoi::Resource>());




	auto boomBox = resourceManager->addResource<ikhanchoi::ModelResource>("boomBox", "BoomBox/BoomBox.gltf");
	auto pbrVert = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrVert", "pbr.vert");
	auto pbrFrag = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrFrag", "pbr.frag");


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	while (!glfwWindowShouldClose(context->getGlfwWindow())) {

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		windowManager->render();

		glfwSwapBuffers(context->getGlfwWindow());



	}




	return 0;
}
