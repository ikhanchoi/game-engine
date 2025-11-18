#pragma once
#include "resource_base.h"

// 지울 예정
class BufferResource;
class MaterialResource;
class SkeletonResource;
class AnimationResource;
class TextureResource;

class ModelResource final : public ResourceBase, public Object {
	bool gpuUploaded = false;

	std::vector<Handle<BufferResource>> vertexBuffers;
	Handle<BufferResource> indexBuffer;

	struct SubMesh {
		uint32_t startIndex;
		uint32_t indexCount;
		Handle<MaterialResource> material;
	};
	std::vector<SubMesh> subMeshes;

	Handle<SkeletonResource> skeleton;
	std::vector<Handle<AnimationResource>> animations;
	std::vector<Handle<TextureResource>> textures;
};