#pragma once

class BufferResource;

 // MeshResource does not depend on each object of MeshComponent
 // MeshInstance depends on each object of MeshComponent


struct SubmeshResource final : ResourceBase, Object {
	uint32_t indexOffset;
	uint32_t indexCount;
	uint32_t materialInstanceIndex;
};


struct MeshResource final : ResourceBase, Object {
	std::vector<Handle<BufferResource>> vertexBuffers;
	std::optional<Handle<BufferResource>> indexBuffer;

	uint32_t vertexStride = 0;
	uint32_t vertexCount = 0;
	uint32_t indexCount = 0;

	std::vector<Handle<SubmeshResource>> submeshes;
};