#include "resource.h"
#include "window.h"

// setter getter, adder remover, loader unloader, updater shower
// update internal after set
/*
 *
 *
 * how to update systematically....
 * event system: update or add
 *
 *
 */

int main() {

	auto window = new ikhanchoi::Window("GLTF Viewer", 1280, 960);
	window->initialize();

	auto resourceManager = new ikhanchoi::ResourceManager();

	window->setResourceManager(resourceManager);

	auto boomBox = resourceManager->addResource("BoomBox/BoomBox.gltf", typeid(ikhanchoi::ModelResource));
	auto pbrVert = resourceManager->addResource("pbr.vert", typeid(ikhanchoi::ShaderResource));
	auto pbrFrag = resourceManager->addResource("pbr.frag", typeid(ikhanchoi::ShaderResource));




	while (!window->shouldClose()) {

		window->pollEvents();


		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


		window->show();


		window->swapBuffers();
	}

	window->terminate();

	delete window;
	delete resourceManager;

	return 0;
}
