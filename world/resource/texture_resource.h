#pragma once
#include "world/resource/resource_base.h"

class TextureResource final : public ResourceBase, public Object {
	std::vector<unsigned char> data;
	uint32_t width, height, channels;
};