#pragma once
#include "world/component/component_base.h"

class MaterialResource;
class MaterialInstance;

struct MeshComponent final : ComponentBase, Object {
	Handle<MeshResource> mesh;
	Handle<MaterialResource> material;
	Handle<MaterialInstance> materialInstance;
};