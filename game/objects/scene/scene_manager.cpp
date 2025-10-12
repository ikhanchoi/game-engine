#include "scene_manager.h"
#include "scene.h"
#include "persistent_scene.h"
#include "scene_events.h"
#include "../../../core/memory/allocators/naive_allocator.h"
#include "../../../core/memory/allocators/pool_allocator.h"
#include "core/execution/commands/command.h"

SceneManager::SceneManager(World& world) : ManagerBase(world) {
	registerStorage<Scene, NaiveAllocator>(); // TODO: naive should be changed to stack
	registerStorage<PersistentScene, NaiveAllocator>();

	create<PersistentScene>();
}

Handle<Scene> SceneManager::loadScene(const std::string& path) {
	return create<Scene>(); // TODO
}

Handle<Scene> SceneManager::loadAdditiveScene(const std::string& path) {
	return create<Scene>(); // TODO
}

void SceneManager::unloadScene(Handle<Scene> scene) {
	push<Command>(
		[this, scene] { _unloadScene(scene); }
	);
}

void SceneManager::saveScene(Handle<Scene> scene, const std::string& path) {
	push<Command>(
		[this, scene, path] { _saveScene(scene, path); }
	);
}




Handle<Scene> SceneManager::newScene(const std::string& name) {
	auto newScene = create<Scene>();
	push<Command>(
		[this, name, newScene] {
			for (auto scene : view<Scene>()) {
				if (scene == newScene) continue;
				_saveScene(scene);
				_unloadScene(scene);
			}
			resolve<Scene>(newScene)->name = name.empty() ? name : "Scene " + std::to_string(newScene.id);
			resolve<PersistentScene>(getPersistentScene())->mainScene = newScene;
		}
	);
	return newScene;
}





Handle<PersistentScene> SceneManager::getPersistentScene() {
	const auto& persistentScenes = view<PersistentScene>();
	if (persistentScenes.size() != 1)
		throw std::runtime_error("Error: (SceneManager::getPersistentScene) PersistentScene count is not 1.");
	return persistentScenes.front();
}

std::optional<Handle<Scene>> SceneManager::getMainScene() {
	return resolve<PersistentScene>(getPersistentScene())->mainScene;
}

void SceneManager::setMainScene(Handle<Scene> scene) {
	auto oldScene = getMainScene();
	if (oldScene == scene) return;
	if (!valid(scene))
		throw std::runtime_error("Invalid scene handle.");
	push<Command>(
		[this, oldScene, scene] {
			resolve<PersistentScene>(getPersistentScene())->mainScene = scene;
			publish<MainSceneChangedEvent>(oldScene, scene);
		}
	);
}





void SceneManager::_saveScene(Handle<Scene> scene, const std::string& path) {
	// TODO;
}

void SceneManager::_unloadScene(Handle<Scene> scene) {
	destroy(scene);
}