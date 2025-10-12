#pragma once
#include <string>
#include <fstream>
#include <sstream>


#include <GL/glew.h>
#include "tiny_gltf.h"


class ResourceBase {
	void* iconTexture = nullptr;
public:
	virtual ~ResourceBase() = default;
	virtual void loadResource(const std::string& path) = 0; // path after ../resources/models/ or ../resources/shaders/

	void* getIconTexture() const { return iconTexture; }
};


class ModelResource : public ResourceBase {
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects;
	std::vector<GLuint> textureObjects;
public:
	explicit ModelResource(uint32_t id, const std::string& name) {}
	~ModelResource() override;

	void loadResource(const std::string& path) override {
		loadModel("../resources/models/" + path);
		loadBufferObjects();
		loadTextureObjects();
	}
	void loadModel(const std::string& path);
	void loadBufferObjects();
	void loadTextureObjects();

	const tinygltf::Model& getModel() const { return model; }
	const std::vector<GLuint>& getBufferObjects() const { return bufferObjects; }
	const std::vector<GLuint>& getTextureObjects() const { return textureObjects; }

	static std::string getTypeName() { return "Model"; }

};

/*
 * RAII is not applied directly in the constructor, but in the adder method of the manager.
 */

// external model
// primitive model (pbr factor, mesh type, etc.)
// implicit model
// sdf(signed distance function)? csg(constructive solid geometry)?



class ShaderResource : public ResourceBase {
	GLuint shaderObject;
public:
	explicit ShaderResource(unsigned int id, const std::string& name) {}
	~ShaderResource() override;

	static std::string getTypeName() { return "Shader"; }

	void loadResource(const std::string& path) override {
		loadShader("../resources/shaders/" + path);
	}
	void loadShader(const std::string& path);

	GLuint getShaderObject() const { return shaderObject; }


};
/*
class ResourceManager : public PoolManagerBase {
public:
	explicit ResourceManager(Context* context) : PoolManagerBase(context) {}


	void subscribe() override {};

	Handle addResource(const std::type_index& resourceType, const std::string& name, const std::string& path) {
		if (pool.find(resourceType) == pool.end())
			throw std::runtime_error("Error: (ResourceManager::addResource) Resource type not registered");
		auto handle = create(resourceType, name);
		this->access<ResourceBase>(handle)->loadResource(path);
		return handle;
	}
	template <typename ResourceType>
	Handle addResource(const std::string& name, const std::string& path) {
		static_assert(std::is_base_of_v<ResourceBase, ResourceType>);
		return addResource(typeid(ResourceType), name, path);
	}
};

*/

