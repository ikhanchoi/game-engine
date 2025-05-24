#include "resource.h"
#include "window.h"

int main() {

	auto window = new ikhanchoi::Window("GLTF Viewer", 1280, 960);
	window->initialize();

	auto resourceManager = new ikhanchoi::ResourceManager();

	window->setResourceManager(resourceManager);

	auto boomBox = resourceManager->addResource<ikhanchoi::ModelResource>("BoomBox/BoomBox.gltf");
	auto pbrVert = resourceManager->addResource<ikhanchoi::ShaderResource>("pbr.vert");
	auto pbrFrag = resourceManager->addResource<ikhanchoi::ShaderResource>("pbr.frag");




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
