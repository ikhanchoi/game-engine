#pragma once
#include "world/manager_base.h"

class EntityManager final : public ManagerBase {
	class SceneManager& sceneManager;

public:
	explicit EntityManager(World& world);

	void onCurrentSceneChanged(const class CurrentSceneChangedEvent&);

	Handle<Entity> addEntity(const std::string& name = "",
		std::optional<Handle<Entity>> parent = std::nullopt,
		std::optional<Handle<Scene>> scene = std::nullopt);
	void removeEntity(Handle<Entity> entity,
		std::optional<Handle<Scene>> scene = std::nullopt);
//	void moveEntity(Handle<Entity> entity,
//		std::optional<Handle<Entity>> parent = std::nullopt,
//		std::optional<Handle<Scene>> scene = std::nullopt); // move to different scene?

	~EntityManager() override = default;
};
