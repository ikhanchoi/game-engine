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
	unsigned int id; // id counted for each type
	std::string name; // path after ../assets/models/ or ../assets/shaders/
	std::type_index type;
	bool active = true;
public:
	explicit Resource(unsigned int id, std::string name, const std::type_index& type)
		: id(id), name(std::move(name)), type(type) {}
	virtual ~Resource() = default;

	void setId(unsigned int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setType(const std::type_index& type) { this->type = type; }
	void setActive(bool active) { this->active = active; }
	unsigned int getId() const { return id; }
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
	explicit ModelResource(unsigned int id, const std::string& name)
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
	explicit ShaderResource(unsigned int id, const std::string& name)
		: Resource(id, name, typeid(ShaderResource)) {
		loadShader();
	}
	~ShaderResource() override;

	GLuint getShader() const { return shader; }

	void loadShader();

	void show() override;

};



/*  */




class ResourcePoolBase {
public:
	virtual void forEach(const std::function<void(Resource&)>& function) = 0;
    virtual ~ResourcePoolBase() = default;
	virtual const unsigned int getSize() const = 0;
	virtual std::shared_ptr<Resource> addResource(std::unique_ptr<Resource> resource) = 0;
};

template <typename T>
class ResourcePool : public ResourcePoolBase {
private:
	std::vector<T> resources;
public:
	explicit ResourcePool() {
		static_assert(std::is_base_of<Resource, T>::value);
	}
	~ResourcePool() override = default;
	void forEach(const std::function<void(Resource&)>& function) override {
		for (auto& resource : resources)
			if (resource.isActive())
				function(resource);
	}

	const unsigned int getSize() const override {
		return static_cast<unsigned int>(resources.size());
	}
	std::vector<T>& getResources() { // no const!
		return resources;
	}

	std::shared_ptr<Resource> addResource(std::unique_ptr<Resource> resource) override {
		auto* casted = dynamic_cast<T*>(resource.release());
		if (!casted)
			throw std::runtime_error("Resource type mismatch in ResourcePool");
		resources.push_back(std::move(*casted));
		return std::make_shared<T>(resources.back());
	}
	void removeResource(unsigned int id) {
		assert(id < resources.size());
		resources[id].setActive(false);
	}
};

static const std::unordered_map<std::type_index, std::string> resourceTypeNames
	= {{typeid(ModelResource), "Model"},
	   {typeid(ShaderResource), "Shader"}};



class ResourceManager { // not considered memory management yet
private:
	std::unordered_map<std::type_index, std::unique_ptr<ResourcePoolBase>> resourcePool;
	std::unordered_map<std::type_index, std::function<std::unique_ptr<Resource>(unsigned int, const std::string&)>> factory;

public:
	template <typename T>
	void registerType() {
		static_assert(std::is_base_of<Resource, T>::value);
		factory[typeid(T)] = [](unsigned int id, const std::string& name) {
			return std::make_unique<T>(id, name);
		};
		if (!resourcePool.contains(typeid(T)))
			resourcePool[typeid(T)] = std::make_unique<ResourcePool<T>>();
	}
	explicit ResourceManager() {
		registerType<ModelResource>();
		registerType<ShaderResource>();
	};
	~ResourceManager() = default;
	void forEach(const std::type_index& type, const std::function<void(Resource&)>& function) {
		resourcePool[type]->forEach(function);
	}

	template <typename T>
	std::vector<T>& getResources(std::type_index type) {
		static_assert(std::is_base_of<Resource, T>::value);
		return dynamic_cast<ResourcePool<T>*>(resourcePool[type].get())->getResources();
	}

	std::shared_ptr<Resource> addResource(const std::string& name, const std::type_index type) {
		unsigned int id = resourcePool[type]->getSize();
		return resourcePool[type]->addResource(std::move(factory[type](id, name)));
	}

	void show() {
		ImGui::PushID(this);
		ImGui::Begin("Resource manager");
		ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
		for (const auto& [type, typeName] : resourceTypeNames) {
			ImGui::PushID(type.name());
			if (ImGui::BeginTabItem((typeName + "s").c_str())) {
				forEach(type, [](Resource& resource) {
					resource.show();
				});
				ImGui::EndTabItem();
			}
			ImGui::PopID();
		}
		ImGui::EndTabBar();
		ImGui::End();
		ImGui::PopID();
	}
};


}



#endif//GAME_ENGINE_RESOURCE_H
