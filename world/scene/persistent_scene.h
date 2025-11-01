#pragma once

#include "scene_base.h"

class Scene;

class PersistentScene final : public SceneBase, public Object {
	std::optional<Handle<Scene>> mainScene;

public:
	explicit PersistentScene(uint32_t id) : Object(id) {}

	friend class SceneManager;
};