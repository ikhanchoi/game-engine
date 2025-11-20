#include "scene_manager.h"
#include "core/execution/command/command.h"
#include "scene.h"
#include "scene_events.h"

SceneManager::SceneManager(World& world) : ManagerBase(world) {
	registerStorage<Scene>(); // TODO: naive should be changed to stack

	persistentScene = std::make_unique<Handle<Scene>>(create<Scene>());
}

Handle<Scene> SceneManager::loadScene(const std::string& path) {
	return create<Scene>(); // TODO
}

Handle<Scene> SceneManager::loadAdditiveScene(const std::string& path) {
	return create<Scene>(); // TODO
}

void SceneManager::unloadScene(Handle<Scene> scene) {
}

void SceneManager::saveScene(Handle<Scene> scene, const std::string& path) {
}





void SceneManager::setCurrentScene(std::optional<Handle<Scene>> scene) {
	submit([this, scene] {
		if (currentScene == scene) return;
		if (scene.has_value() && !valid(scene.value()))
			throw std::runtime_error("Error: (SceneManager::setCurrentScene) The specified scene is not empty but also not valid.");
		publish<CurrentSceneChangedEvent>(currentScene, scene);
		currentScene = scene;
	});
}

void SceneManager::setEntityName(Handle<Entity> entity, const std::string& name, std::optional<Handle<Scene>> scene) {
	submit([this, entity, name, scene] {
		auto actualScene = scene.has_value() ? scene.value() : currentScene.has_value() ? currentScene.value()
								: throw std::runtime_error("Error: (SceneManager::getSceneGraph) The current scene is not set.");
		resolve<Scene>(actualScene)->entityName[entity] = name;
	});
}



Handle<Scene> SceneManager::newScene(const std::string& name) {
	for (auto scene : view<Scene>()) {
		saveScene(scene);
		unloadScene(scene);
	}
	auto scene = create<Scene>();
	if (!name.empty())
		resolve<Scene>(scene)->name = name;
	setCurrentScene(scene);
	return scene;
}






// query responders

Handle<Scene> SceneManager::getPersistentScene() {
	return *persistentScene;
}

Forest<Handle<Entity>>* SceneManager::getEntityGraph(std::optional<Handle<Scene>> scene) {
	if (!scene.has_value()) {
		if (!getCurrentScene().has_value())
			throw std::runtime_error("Error: (SceneManager::getSceneGraph) The main scene is not set.");
		scene = getCurrentScene();
	}
	return &resolve<Scene>(scene.value())->entityGraph;
}

std::optional<Handle<Scene>> SceneManager::getCurrentScene() {
	return currentScene;
}

const std::string& SceneManager::getEntityName(Handle<Entity> entity, std::optional<Handle<Scene>> scene) {
	if (!scene.has_value()) {
		if (!currentScene.has_value())
			throw std::runtime_error("Error: (SceneManager::getSceneGraph) The main scene is not set.");
		scene = currentScene;
	}
	auto& entityNameMap = resolve<Scene>(scene.value())->entityName;
	if (entityNameMap.contains(entity))
		return entityNameMap[entity];
	static const std::string empty = "";
	return empty;
}
