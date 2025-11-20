#pragma once
#include <glm/glm.hpp>

struct MeshInstance {
	uint32_t meshResourceIndex;
	uint32_t materialStartIndex;
	glm::mat4 worldMatrix;
};

