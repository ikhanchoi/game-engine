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
	explicit Resource(unsigned int id, const std::string& name);
	virtual void loadResource(const std::string& path) = 0; // path after ../assets/models/ or ../assets/shaders/
};


class ModelResource : public Resource {
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects;
	std::vector<GLuint> textureObjects;
public:
	explicit ModelResource(unsigned int id, const std::string& name) : Resource(id, name) {}
	~ModelResource() override;
	void update(Updater& updater) override { updater.update(*this); }
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
	void update(Updater& updater) override { updater.update(*this); }
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
	explicit ResourceManager(unsigned int id, const std::string& name);
	~ResourceManager() override;
	void update(Updater& updater) override { updater.update(*this); }

	std::shared_ptr<Resource> addResource(const std::type_index& resourceType, const std::string& name, const std::string& path) {
		if (types.find(resourceType) == types.end())
			throw std::runtime_error("Error: (ResourceManager::addResource) Resource type not registered");
		auto resource = dynamic_cast<Resource*>(access(create(resourceType, name)));
		resource->loadResource(path);
		std::shared_ptr<void> dummy(nullptr, [](void*) {});
		std::shared_ptr<Resource> alias(dummy, resource);
		return alias;
	}
	template <typename ResourceType>
	std::shared_ptr<ResourceType> addResource(const std::string& name, const std::string& path) {
		return dynamic_pointer_cast<ResourceType>(addResource(typeid(ResourceType), name, path));
	}
};


}



#endif//GAME_ENGINE_RESOURCE_H
