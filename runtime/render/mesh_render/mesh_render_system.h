#pragma once
#include "world/system_base.h"

struct MeshInstance;
struct MaterialInstance;
struct MeshResource;
struct MeshComponent;
struct TransformComponent;




class MeshRenderSystem final : public SystemBase {
	class ResourceManager& resourceManager;
	class TransformSystem& transformSystem;

	std::vector<MeshInstance> meshInstances;
	std::vector<MaterialInstance> materialInstances;
	std::unordered_map<Handle<MeshResource>, uint32_t> meshResourceIndexTable;
	std::unordered_map<Handle<MaterialInstance>, uint32_t> materialInstanceIndexTable;

public:
	explicit MeshRenderSystem(World& world);

	void tick() override;
	void flattenMeshes();
	void flattenMaterials();
	void draw();
};