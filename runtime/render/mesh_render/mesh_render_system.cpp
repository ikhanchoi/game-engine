#include "mesh_render_system.h"
#include "../../../world/component/transform/transform_system.h"
#include "mesh_component.h"
#include "mesh_instance.h"
#include "runtime/render/material/material_instance.h"

#include "world/resource/resource_manager.h"

#include <GL/glew.h>

MeshRenderSystem::MeshRenderSystem(World& world) : SystemBase(world),
	resourceManager(*world.get<ResourceManager>()),
	transformSystem(*world.get<TransformSystem>()) {
	registerStorage<MeshComponent>();

}


void MeshRenderSystem::tick() {
	draw();
}



void MeshRenderSystem::flattenMeshes() {
	meshInstances.clear();
	meshInstances.reserve(view<MeshComponent>().size()); // TODO: optimized usage of view?
	each<MeshComponent>([this](Handle<Entity> entity, const MeshComponent* mesh) {
		MeshInstance meshInstance = {
			meshResourceIndexTable.at(mesh->mesh),
			materialInstanceIndexTable.at(mesh->materialInstance),
			transformSystem.getWorldMatrix(entity)
		};
		meshInstances.push_back(meshInstance);
	});
}

void MeshRenderSystem::flattenMaterials() {
	materialInstances.clear();
	materialInstanceIndexTable.clear();
	each<MeshComponent>([this](const MeshComponent* mesh) {
		uint32_t idx = 0;
		MaterialInstance matInst;
		// TODO: uniform reflection이 없으므로 최소한의 shader/texture만 채움
		matInst.shaderProgram = mesh->material.getShaderProgramGPUHandle();
		matInst.uboHandle = mesh->material.getDefaultUBO();
		matInst.textureHandle0 = mesh->material.textures.size() > 0
			? mesh->material.textures[0]->gpuHandle.getAPIHandle() : 0;
		matInst.textureHandle1 = mesh->material.textures.size() > 1
			? mesh->material.textures[1]->gpuHandle.getAPIHandle() : 0;
		materialInstances.push_back(matInst);
		// CPU-side MaterialInstance* → Flattened GPU index
		materialInstanceIndexTable[mesh->material] = idx++;
	});
}

void MeshRenderSystem::draw() {/*
	submit<Command>([this] {
		each<MeshComponent>([](MeshComponent* meshComponent) {
			auto& model = meshComponent->model;
			auto& material = meshComponent->material;
			bindShader(material->shader);
			for (size_t i = 0; i < model->submeshes.size(); ++i) {
				auto& submesh = model->submeshes[i];
				glBindVertexArray(submesh->vao);
				bindMaterial(material, i);
				glDrawElements(
					submesh->primitiveType,
					submesh->indexCount,
					GL_UNSIGNED_INT,
					nullptr
				);
			}
		});
	});*/
}
