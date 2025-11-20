#include "transform_system.h"
#include "transform_component.h"

#include "world/scene/scene_manager.h"

TransformSystem::TransformSystem(World& world) : SystemBase(world),
	sceneManager(world.get<SceneManager>()) {
	registerStorage<TransformComponent>();
}