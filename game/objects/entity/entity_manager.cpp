#include "entity_manager.h"
#include "core/memory/allocators/naive_allocator.h"
#include "entity.h"
#include "game/objects/scene/scene.h"
#include "game/objects/scene/scene_events.h"
#include "game/objects/scene/scene_manager.h"
#include "core/execution/commands/command.h"


EntityManager::EntityManager(World& world) : ManagerBase(world) {
	registerStorage<Entity, NaiveAllocator>();

	push<Command>(
		[this] {
			get<SceneManager>()->subscribe<MainSceneChangedEvent>( // TODO: subscription may be able to be automatically using reflection.
				[this](const MainSceneChangedEvent& event){ onMainSceneChanged(event); }
			);
		}
	);

}


void EntityManager::onMainSceneChanged(const MainSceneChangedEvent& event) {
	push<Command>(
		[this, event] {
			if (event.newScene)
				_setCachedMainSceneGraph(event.newScene.value());
		}
	);
}

Handle<Entity> EntityManager::addEntity(std::optional<Handle<Entity>> parent, std::optional<Handle<Scene>> scene) { // TODO: entity name or icon?
	// if scene is not specified, it means the main.
	Forest<Handle<Entity>>* sceneGraph;
	if (scene.has_value())
		sceneGraph = &get<SceneManager>()->resolve<Scene>(scene.value())->sceneGraph;
	else {
		if (!cachedMainSceneGraph)
			throw std::runtime_error("Error: (EntityManager::addEntity) The main scene is not cached.");
		sceneGraph = cachedMainSceneGraph;
	}

	const auto entity = create<Entity>();
	push<Command>(
		[sceneGraph, entity, parent] {
			sceneGraph->add(entity, parent);
		}
	);
	return entity;
}


void EntityManager::_setCachedMainSceneGraph(Handle<Scene> scene) {
	cachedMainSceneGraph = &get<SceneManager>()->resolve<Scene>(scene)->sceneGraph;
}
