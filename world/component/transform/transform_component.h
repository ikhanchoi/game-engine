#pragma once
#include "../component_base.h"
#include <glm/gtc/quaternion.hpp>

struct TransformComponent final : ComponentBase, Object {
	glm::vec3 position;
	glm::quat rotation;
	glm::vec3 scale;

	explicit TransformComponent() :
		position(0.0f, 0.0f, 0.0f),
		rotation(1.0f, 0.0f, 0.0f, 0.0f),
		scale(1.0f, 1.0f, 1.0f) {}
};
