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

class Resource : public Object {
public:
	explicit Resource(unsigned int id, const std::string& name) : Object(id, name) {}
	static std::string getTypeName() { return "Resource"; }
	static std::shared_ptr<Manager> generateManager() {
		return std::move(std::dynamic_pointer_cast<Manager>(std::make_shared<ResourceManager>())); }

	virtual void loadResource(const std::string& path) = 0; // path after ../assets/models/ or ../assets/shaders/
};


class ModelResource : public Resource {
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects;
	std::vector<GLuint> textureObjects;
public:
	explicit ModelResource(unsigned int id, const std::string& name) : Resource(id, name) {}
	~ModelResource() override;
	void visit(Visitor& visitor) override { visitor.visit(*this); }
	static std::string getTypeName() { return "Model"; }

	void loadResource(const std::string& path) override {
		loadModel("../assets/models/" + path);
		loadBufferObjects();
		loadTextureObjects();
	}

	const tinygltf::Model& getModel() const { return model; }
	const std::vector<GLuint>& getBufferObjects() const { return bufferObjects; }
	const std::vector<GLuint>& getTextureObjects() const { return textureObjects; }

	void loadModel(const std::string& path);
	void loadBufferObjects();
	void loadTextureObjects();

};

// 모델 리소스를 엔진 내에서 만들 때 파일을 불러올 지 프리미티브로 메시모양과 pbr 팩터를 직접 지정해줄 지 선택
//
// external model과 primitive model


class ShaderResource : public Resource {
	GLuint shader{};
public:
	explicit ShaderResource(unsigned int id, const std::string& name) : Resource(id, name) {}
	~ShaderResource() override;
	void visit(Visitor& visitor) override { visitor.visit(*this); }
	static std::string getTypeName() { return "Shader"; }

	void loadResource(const std::string& path) override {
		loadShader("../assets/shaders/" + path);
	}

	GLuint getShader() const { return shader; }

	void loadShader(const std::string& path);

};



/*-----------------*/
/* ResourceManager */
/*-----------------*/




class ResourceManager : public Manager {
public:
	explicit ResourceManager() : Manager(0, "Resource manager", typeid(Resource)) {}
	~ResourceManager() override = default;
	void visit(Visitor& visitor) override { visitor.visit(*this); }

	Handle addResource(const std::type_index& resourceConcrete, const std::string& name, const std::string& path) {
		if (pool.find(resourceConcrete) == pool.end())
			throw std::runtime_error("Error: (ResourceManager::addResource) Resource type not registered");
		auto handle = create(resourceConcrete, name);
		this->access<Resource>(handle)->loadResource(path);
		return handle;
	}
	template <typename ResourceType>
	Handle addResource(const std::string& name, const std::string& path) {
		static_assert(std::is_base_of_v<Resource, ResourceType>);
		return addResource(typeid(ResourceType), name, path);
	}
};


}



#endif//GAME_ENGINE_RESOURCE_H
