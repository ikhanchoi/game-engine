#pragma once
#include "world/resource/resource_base.h"

struct TextureResource final : ResourceBase, Object {
	std::vector<unsigned char> data;
	uint32_t width, height, channels;
	std::optional<GPUHandle> gpuHandle;
};