#pragma once
#include "scene_base.h"

class Scene final : public SceneBase, public Object {
	friend class SceneManager;

	std::string path;
	std::string name; // for editor

public:
	explicit Scene(uint32_t id) : Object(id) {}
};