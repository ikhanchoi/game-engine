#pragma once
#include "world/manager_base.h"
#include "core/containers/tree.h"

class SceneManager final : public ManagerBase {
	std::unique_ptr<Handle<Scene>> persistentScene;
	std::optional<Handle<Scene>> currentScene;

public:
	explicit SceneManager(World& world);


	// command submitters
	Handle<Scene> loadScene(const std::string& path); // TODO: this loads and activate a scene from saved scene data.
	Handle<Scene> loadAdditiveScene(const std::string& path); // TODO: this loads but does not activate a scene from saved scene data.
	void saveScene(Handle<Scene> scene, const std::string& path = ""); // TODO: this saves a scene to scene data.
	void unloadScene(Handle<Scene> scene);

	void setCurrentScene(std::optional<Handle<Scene>> scene);
	void setEntityName(Handle<Entity> entity, const std::string& name, std::optional<Handle<Scene>> scene = std::nullopt); // for editor

	// indirect command submitters
	Handle<Scene> newScene(const std::string& name = ""); // for editor



	// query responders
	Handle<Scene> getPersistentScene();
	std::optional<Handle<Scene>> getCurrentScene();
	Forest<Handle<Entity>>* getEntityGraph(std::optional<Handle<Scene>> scene = std::nullopt);
	const std::string& getEntityName(Handle<Entity> entity, std::optional<Handle<Scene>> scene = std::nullopt);


};
