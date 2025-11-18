#include "editor/editor.h"
#include "world/entity/entity_manager.h"
#include "world/scene/scene_manager.h"
#include "world/resource/model_resource.h"

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

	TIME_BLOCK_START(1);
	const auto editor = std::make_unique<Editor>();
	TIME_BLOCK_END(1);

	auto* editWorld = editor->getEditWorld();
	auto* sceneManager = editWorld->get<SceneManager>();
	auto* entityManager = editWorld->get<EntityManager>();


	auto scene0 = sceneManager->newScene();
	auto entity0 = entityManager->addEntity();
	auto entity1 = entityManager->addEntity();
	entityManager->removeEntity(entity0);


	editor->loop();

	return 0;
}
