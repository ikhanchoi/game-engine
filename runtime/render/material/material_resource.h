#pragma once
#include "world/resource/resource_base.h"

struct ShaderResource;
struct TextureResource;
class UniformLayout; // class? struct?

struct MaterialResource final : ResourceBase, Object {
	Handle<ShaderResource> vertexShader;
	Handle<ShaderResource> fragmentShader; // TODO: shader graph
	std::vector<Handle<TextureResource>> textures;
	std::vector<UniformLayout> uniformLayout; // TODO: reflection
};







