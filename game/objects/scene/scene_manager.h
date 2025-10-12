#pragma once
#include "game/manager_base.h"

class Scene;
class PersistentScene;

class SceneManager final : public ManagerBase {
	// TODO: methods will be distributed to the following subsystems.
	// std::unique_ptr<SceneLoader>
	// std::unique_ptr<SceneSerializer> serializer: serialization type like json or binary
	// std::unique_ptr<SceneStreamer>
	// etc

public:
	explicit SceneManager(World& world);

	Handle<Scene> loadScene(const std::string& path); // TODO: this loads and activate a scene from saved scene data.
	Handle<Scene> loadAdditiveScene(const std::string& path); // TODO: this loads but does not activate a scene from saved scene data.
	void saveScene(Handle<Scene>, const std::string& path = ""); // TODO: this saves a scene to scene data.
	void unloadScene(Handle<Scene>);

	Handle<Scene> newScene(const std::string& name = ""); // for editor

	Handle<PersistentScene> getPersistentScene();
	std::optional<Handle<Scene>> getMainScene();
	void setMainScene(Handle<Scene>);

private:
	void _saveScene(Handle<Scene>, const std::string& path = "");
	void _unloadScene(Handle<Scene>);
};
