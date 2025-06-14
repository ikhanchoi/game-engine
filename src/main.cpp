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

	// Modules
	auto windowManager = context->registerManager<ikhanchoi::WindowManager>("Window manager");
	auto resourceManager = context->registerManager<ikhanchoi::ResourceManager>("Resource manager");
	auto entityManager = context->registerManager<ikhanchoi::EntityManager>("Entity manager");

	windowManager->registerType<ikhanchoi::Window>("Window");

	resourceManager->registerType<ikhanchoi::ModelResource>("Model");
	resourceManager->registerType<ikhanchoi::ShaderResource>("Shader");

	// entityManager->registerType<ikhanchoi::Entity>("Entity");


	// Settings

	auto resourceManagerWindow = windowManager->addWindow("Resource manager", resourceManager);
	auto entityManagerWindow = windowManager->addWindow("Entity manager", entityManager);


	auto boomBox = resourceManager->addResource<ikhanchoi::ModelResource>("boomBox", "BoomBox/BoomBox.gltf");
	auto pbrVert = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrVert", "pbr.vert");
	auto pbrFrag = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrFrag", "pbr.frag");


	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	while (!glfwWindowShouldClose(context->getGlfwWindow())) {

		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		windowManager->update();

		glfwSwapBuffers(context->getGlfwWindow());



	}




	return 0;
}
