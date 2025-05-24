#ifndef GAME_ENGINE_RESOURCE_H
#define GAME_ENGINE_RESOURCE_H

#include <string>
#include <typeindex>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"
#include "tiny_gltf.h"

namespace ikhanchoi {

class Resource {
protected:
	int id; // id counted for each type
	std::string name; // path after ../assets/models/ or ../assets/shaders/
	std::type_index type;
	bool active = true;

	Resource(int id, std::string name, const std::type_index& type)
		: id(id), name(std::move(name)), type(type) {}

public:
	virtual ~Resource() = default;
	void setId(int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setType(const std::type_index& type) { this->type = type; }
	void setActive(bool active) { this->active = active; }
	int getId() const { return id; }
	const std::string& getName() const { return name; }
	const std::type_index& getType() const { return type; }
	bool isActive() const { return active; }
	virtual void show() = 0;
};


class ModelResource : public Resource {
private:
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects;
	std::vector<GLuint> textureObjects;
public:
	explicit ModelResource(int id, const std::string& name)
		: Resource(id, name, typeid(ModelResource)) {
		loadModel();
		loadBufferObjects();
		loadTextureObjects();
	}
	~ModelResource() override;

	const tinygltf::Model& getModel() { return model; }
	const std::vector<GLuint>& getBufferObjects() { return bufferObjects; }
	const std::vector<GLuint>& getTextureObjects() { return textureObjects; }

	void loadModel();
	void loadBufferObjects();
	void loadTextureObjects();

	void show() override;
};

class ShaderResource : public Resource {
private:
	GLuint shader{};
public:
	explicit ShaderResource(int id, const std::string& name)
		: Resource(id, name, typeid(ShaderResource)) {
		loadShader();
	}
	~ShaderResource() override;

	GLuint getShader() { return shader; }

	void loadShader();

	void show() override;

};



class ResourceManager { // not considered memory management yet
private:
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<Resource>>> resources; // indexed by type and id

public:
	template <typename T>
	const std::vector<std::shared_ptr<T>>& getResources() { return resources[typeid(T)]; }

	template <typename T>
	std::shared_ptr<T> addResource(const std::string &name) {
		int id;
		if (resources[typeid(T)].empty())
			resources[typeid(T)].push_back(nullptr);
		for (size_t i = 1; i <= resources[typeid(T)].size(); i++) {
			if (resources[typeid(T)].size() == i)
				resources[typeid(T)].push_back(nullptr);
			if (resources[typeid(T)][i] == nullptr) {
				id = i;
				break;
			}
		}
		std::shared_ptr<Resource> resource = std::make_shared<T>(id, name);
		resources[typeid(T)][id] = resource;
		return std::dynamic_pointer_cast<T>(resource);
	}

	void show();
};


}



#endif//GAME_ENGINE_RESOURCE_H
