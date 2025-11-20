#pragma once
#include "core/memory/handle.h"

#include "world/system_base.h"

#include <glm/mat4x4.hpp>

class TransformSystem final : public SystemBase {
	class SceneManager& sceneManager;
	std::vector<bool> dirtyFlags;
	std::vector<glm::mat4> worldMatrices;

public:
	explicit TransformSystem(World& world);

	// query responder
	glm::mat4 getWorldMatrix(Handle<Entity> entity) {
		return worldMatrices[entity.getId<Entity>()];
	}
};