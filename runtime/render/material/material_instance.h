#pragma once

struct MaterialInstance {
	uint32_t shaderProgram;
	uint32_t uboHandle;        // material parameters
	uint32_t textureHandle0;   // diffuse
	uint32_t textureHandle1;   // normal, etc. // maybe overrided
	// other reflected uniform data
};