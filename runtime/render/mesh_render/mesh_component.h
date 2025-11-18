#pragma once
#include "world/component/component_base.h"
#include "world/resource/model_resource.h"
#include "world/resource/material_resource.h"

struct MeshComponent final : ComponentBase, Object {
	Handle<ModelResource> model;
	Handle<MaterialResource> material;
};