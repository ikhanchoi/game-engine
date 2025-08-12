#ifndef GAME_ENGINE_RESOURCE_H
#define GAME_ENGINE_RESOURCE_H

#include <string>
#include <typeindex>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include "tiny_gltf.h"

#include "modules/core.h"

namespace ikhanchoi {

// A module for resources.
class ResourceModule : public CRTPModule<ResourceModule> {
public:
	static std::unique_ptr<ManagerBase> generateManager(Context* context);
};


// An abstract base class for resource objects.
class ResourceBase : public Base {
public:
	virtual void loadResource(const std::string& path) = 0; // path after ../assets/models/ or ../assets/shaders/
};


class ModelResource : public ResourceBase, public CRTPObject<ModelResource> {
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects;
	std::vector<GLuint> textureObjects;
public:
	explicit ModelResource(uint32_t id, const std::string& name) : CRTPObject<ModelResource>(id, name) {}
	~ModelResource() override;

	void loadResource(const std::string& path) override {
		loadModel("../assets/models/" + path);
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



class ShaderResource : public ResourceBase, public CRTPObject<ShaderResource> {
	GLuint shaderObject;
public:
	explicit ShaderResource(unsigned int id, const std::string& name) : CRTPObject<ShaderResource>(id, name) {}
	~ShaderResource() override;

	static std::string getTypeName() { return "Shader"; }

	void loadResource(const std::string& path) override {
		loadShader("../assets/shaders/" + path);
	}
	void loadShader(const std::string& path);

	GLuint getShaderObject() const { return shaderObject; }


};



/*-----------------*/
/* ResourceManager */
/*-----------------*/




class ResourceManager : public ManagerBase {
public:
	explicit ResourceManager(Context* context) : ManagerBase(context) {}

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


}



#endif//GAME_ENGINE_RESOURCE_H
