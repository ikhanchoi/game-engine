#include "entity_manager.h"
#include "core/execution/commands/command.h"
#include "core/memory/allocators/naive_allocator.h"
#include "entity.h"
#include "world/scene/scene.h"
#include "world/scene/scene_events.h"
#include "world/scene/scene_manager.h"


EntityManager::EntityManager(World& world) : ManagerBase(world),
	sceneManager(*world.get<SceneManager>()) {
	registerStorage<Entity>();

	// subscription
	submit<Command>([this, &world] {
		world.get<SceneManager>()->subscribe<CurrentSceneChangedEvent>( // TODO: subscription may be able to be automatically using reflection.
			[this](const CurrentSceneChangedEvent& event){ onCurrentSceneChanged(event); }
		);
	});
}


void EntityManager::onCurrentSceneChanged(const CurrentSceneChangedEvent& event) {
}

Handle<Entity> EntityManager::addEntity(const std::string& name, std::optional<Handle<Entity>> parent, std::optional<Handle<Scene>> scene) { // TODO: entity name or icon?
	auto entity = create<Entity>();
	submit<Command>([this, entity, name, parent, scene] {
		sceneManager.getSceneGraph(scene)->add(entity, parent);
		if (!name.empty())
			sceneManager.setEntityName(entity, name, scene);
	});
	return entity;
}

void EntityManager::removeEntity(Handle<Entity> entity, std::optional<Handle<Scene>> scene) {
	submit<Command>([this, entity, scene] {
		// TODO: remove all components attached to the entity
		if (!sceneManager.getSceneGraph(scene)->remove(entity))
			throw std::runtime_error("Error: (EntityManager::removeEntity) Failed to remove entity from scene graph.");
	});
}
