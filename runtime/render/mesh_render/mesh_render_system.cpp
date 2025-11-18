#include "mesh_render_system.h"
#include "mesh_component.h"

#include <GL/glew.h>

MeshRenderSystem::MeshRenderSystem(World& world) : SystemBase(world) {
	registerStorage<MeshComponent>();

}


void MeshRenderSystem::tick() {
	draw();
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