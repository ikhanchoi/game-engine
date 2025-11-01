#include "editor/editor.h"
#include "world/entity/entity_manager.h"
#include "world/scene/scene_manager.h"
#include <chrono>
#include <iostream>
#include <thread>


#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b
#define TIME_BLOCK_START(id) \
auto CONCAT(__start_, id) = std::chrono::high_resolution_clock::now();
#define TIME_BLOCK_END(id) \
do { \
auto __end = std::chrono::high_resolution_clock::now(); \
auto __elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(__end - CONCAT(__start_, id)); \
std::cout << "duration: " << __elapsed.count() << " ms\n"; \
} while(0)



int main() {


	// instantiate an editor.
	// (or instantiate a debugger/profiler/player)
	TIME_BLOCK_START(1);
	const auto editor = std::make_unique<Editor>();
	TIME_BLOCK_END(1);

	// make a new project (currently context)
	//    (TODO: in editor you can make a new project with editor ui.)
	// this project initializes a (edit) world and a persistent.

	auto* editWorld = editor->getEditWorld();

	auto* sceneManager = editWorld->get<SceneManager>();

	auto* entityManager = editWorld->get<EntityManager>();









	// make scenes in the world
	// make entities and components in the scenes bla-bla.

	auto scene0 = sceneManager->newScene();
	editWorld->flush(); // scene0 is now main.

	auto entity0 = entityManager->addEntity();
	auto entity1 = entityManager->addEntity();
	entityManager->removeEntity(entity0, scene0);

	// for example, we can do: auto mainCamera = componentManager->create<CameraComponent>();
	// TODO: create objects such as scenes and entities by task scheduler.

	// set entities to the scene
	// set parent-child relationship between entities
 // entity1 is a child of entity0.










	// in the loop:
	//   if in edit mode:
	//     call the editor to render the editor ui with Dear ImGui.
	//     call the editor to render the main view with render systems of the engine.
	//   if in play mode:
	//     hmm...


	/*
	//

	auto context = std::make_unique<ikhanchoi::Context>("GLTF Viewer", 1920, 1080);

	auto* eventManager = context->registerManager<ikhanchoi::EventManager>();
	auto* resourceManager = context->registerManager<ikhanchoi::ResourceManager>();
	auto* entityManager = context->registerManager<ikhanchoi::EntityManager>();
	auto* componentManager = context->registerManager<ikhanchoi::ComponentManager>();
	auto* windowManager = context->registerManager<ikhanchoi::WindowManager>();

	auto* windowViewer = context->registerVisitor<ikhanchoi::WindowViewer>();

	resourceManager->registerPool<ikhanchoi::ModelResource>();
	resourceManager->registerPool<ikhanchoi::ShaderResource>();

	windowManager->addWindow<ikhanchoi::MainViewWindow>("Main View Window");
	windowManager->addWindow<ikhanchoi::AssetWindow>("Asset Window");
	windowManager->addWindow<ikhanchoi::HierarchyWindow>("Hierarchy Window");
	windowManager->addWindow<ikhanchoi::InspectorWindow>("Inspector Window");
	windowManager->addWindow<ikhanchoi::StatisticsWindow>("Statistics Window");


	// Add objects



	auto boomBox = resourceManager->addResource<ikhanchoi::ModelResource>("boomBox", "BoomBox/BoomBox.gltf");
	auto pbrVert = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrVert", "pbr.vert");
	auto pbrFrag = resourceManager->addResource<ikhanchoi::ShaderResource>("pbrFrag", "pbr.frag");


	// -----------------
	 */

	editor->loop();



	return 0;
}
