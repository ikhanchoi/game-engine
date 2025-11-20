#pragma once
#include "world/resource/resource_base.h"

class GPUHandle;

struct BufferResource final : ResourceBase, Object {
	std::vector<unsigned char> data;
	std::optional<GPUHandle> gpuHandle;
};