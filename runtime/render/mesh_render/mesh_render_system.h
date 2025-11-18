#pragma once
#include "world/system_base.h"

class MeshRenderSystem final : public SystemBase {

public:
	explicit MeshRenderSystem(World& world);

	void tick() override;


	void draw();

};