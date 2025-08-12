// Modules
#include "core/context/context.h"

#include "modules/resource.h"
#include "modules/event.h"
#include "modules/window.h"
#include "modules/entity.h"

// Systems
#include "systems/render.h"


int main() {

	auto context = std::make_unique<ikhanchoi::Context>("GLTF Viewer", 1280, 960);

	// Module registration
	context->registerManager<ikhanchoi::ResourceManager>();
	auto* resourceManager = context->getManager<ikhanchoi::ResourceManager>();
	context->registerManager<ikhanchoi::EntityManager>();
	auto* entityManager = context->getManager<ikhanchoi::EntityManager>();
	context->registerManager<ikhanchoi::ComponentManager>();
	auto* componentManager = context->getManager<ikhanchoi::ComponentManager>();
	context->registerManager<ikhanchoi::WindowManager>();
	auto* windowManager = context->getManager<ikhanchoi::WindowManager>();

	// Object registration
	resourceManager->registerPool<ikhanchoi::ModelResource>();
	resourceManager->registerPool<ikhanchoi::ShaderResource>();



	// Add objects



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
