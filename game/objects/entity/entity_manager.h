#pragma once
#include "core/containers/tree.h"
#include "game/manager_base.h"
#include "game/objects/scene/scene_events.h"

class Scene;
class Entity;

class EntityManager final : public ManagerBase {
	Forest<Handle<Entity>>* cachedMainSceneGraph = nullptr;
	// Forest<Handle<Scene>>* cachedLoadedSceneGraph = nullptr;

public:
	explicit EntityManager(World& world);

	void onMainSceneChanged(const MainSceneChangedEvent&);

	Handle<Entity> addEntity(
		std::optional<Handle<Entity>> parent = std::nullopt,
		std::optional<Handle<Scene>> scene = std::nullopt);
//	void removeEntity(Handle<Entity> entity,
//		std::optional<Handle<Scene>> scene = std::nullopt);
//	void moveEntity(Handle<Entity> entity,
//		std::optional<Handle<Entity>> parent = std::nullopt,
//		std::optional<Handle<Scene>> scene = std::nullopt); // move to different scene?

	~EntityManager() override = default;
private:
	void _setCachedMainSceneGraph(Handle<Scene>);
};
