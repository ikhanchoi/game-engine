#pragma once
#include "world/resource/resource_base.h"

class BufferResource final : public ResourceBase, public Object {
	std::vector<unsigned char> data;
};