#ifndef GLTF_VIEWER_IKHANCHOI_H
#define GLTF_VIEWER_IKHANCHOI_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <typeindex>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"



// setter getter, adder remover, loader unloader, updater shower
// update internal after set
/*
 *
 *
 * how to update systematically....
 * event system: update or add
 *
 *
 */

// TODO: why doesn't the metallic texture of the damaged helmet work?



namespace ikhanchoi {

class Resource;
class ModelResource;
class ShaderResource;
class ResourceManager;

class Component;
class RenderComponent;
class TransformComponent;
class CameraComponent;
class LightComponent;
class ComponentManager;

class Entity;
class EntityManager;

class RenderSystem;






/*-------------*/
/*  Resources  */
/*-------------*/


class Resource {
protected:
	int id; // count id for each type
	std::string name;
	std::string extension;
public:
	Resource(const std::string& name, const std::string& extension) : name(name), extension(extension) {}
	virtual ~Resource() {}

	void setId(int id) { this->id = id; }
	int getId() const { return id; }
	const std::string& getName() const { return name; }
	const std::string& getExtension() const { return extension; }
};




class ModelResource : public Resource {
private:
	tinygltf::Model model;
	std::vector<GLuint> bufferObjects; // indexed by gltf id
	std::vector<GLuint> textureObjects; // indexed by gltf id

public:
	explicit ModelResource(const std::string& name, const std::string& extension) : Resource(name, extension) {
		// load model
		std::string path = "../assets/models/" + name + "/" + name + "." + extension;
		std::string err, warn;
		bool ret = false;
		tinygltf::TinyGLTF loader;
		if (extension == "gltf")
			ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
		else if (extension == "glb")
			ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
		if (!warn.empty()) std::cout << "Warning: " << warn << std::endl;
		if (!err.empty())  std::cout << "Error: " << err << std::endl;
		if (!ret)          std::cerr << "Error: Failed to parse gltf file." << std::endl;

		for (const auto& buffer: model.buffers) {
			GLuint bufferObject;
			glGenBuffers(1, &bufferObject);
			glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
			glBufferData(GL_ARRAY_BUFFER, (GLsizei) buffer.data.size(), &buffer.data.at(0), GL_STATIC_DRAW);
			bufferObjects.push_back(bufferObject);
		}

		for (const auto& image : model.images) {
			GLuint textureObject;
			glGenTextures(1, &textureObject);
			glBindTexture(GL_TEXTURE_2D, textureObject);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
						 &image.image.at(0));
			glGenerateMipmap(GL_TEXTURE_2D);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			textureObjects.push_back(textureObject);
		}
	}

	~ModelResource() override {
		for (auto& bufferObject : bufferObjects)
			glDeleteBuffers(1, &bufferObject);
		for (auto& textureObject : textureObjects)
			glDeleteTextures(1, &textureObject);
	}


	const tinygltf::Model& getModel() { return model; }
	const std::vector<GLuint>& getBufferObjects() { return bufferObjects; }
	const std::vector<GLuint>& getTextureObjects() { return textureObjects; }
};



class ShaderResource : public Resource {
private:
	GLuint shader;
public:
	explicit ShaderResource(const std::string& name, const std::string& extension) : Resource(name, extension) {
		std::ifstream fs;
		std::string code;
		const char* cstr;
		int success;

		fs.exceptions(std::ifstream::failbit|std::ifstream::badbit);
		try {
			fs.open("../assets/shaders/" + name + "." + extension);
			std::stringstream ss;
			ss << fs.rdbuf();
			code = ss.str();
			fs.close();
		} catch (std::ifstream::failure& e) {
			std::cout << "ERROR::SHADER_NOT_LOADED: " << e.what() << "\n";
		}
		cstr = code.c_str();

		if (extension == "vert") shader = glCreateShader(GL_VERTEX_SHADER);
		else if (extension == "frag") shader = glCreateShader(GL_FRAGMENT_SHADER);
		else std::cerr << "Shader Type Error: " << extension << std::endl;

		glShaderSource(shader, 1, &cstr, NULL);
		glCompileShader(shader);
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			char infoLog[512];
			glGetShaderInfoLog(shader, 512, NULL, infoLog);
			std::cerr << "Shader Compilation Error: " << infoLog << std::endl;
		}
	}

	~ShaderResource() override { glDeleteShader(shader); }

	GLuint getShader() { return shader; }

};



class ResourceManager {
private:
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<Resource>>> resources; // indexed by type and id


	std::unordered_map<std::type_index, std::string> typeNames
		= {{typeid(ModelResource), "Model"},
		   {typeid(ShaderResource), "Shader"}};
public:

	template <typename T>
	std::shared_ptr<T> addResource(const std::string& name, const std::string& extension) {
		std::shared_ptr<Resource> resource = std::make_shared<T>(name, extension);
		if (resources[typeid(T)].empty()) resources[typeid(T)].push_back(nullptr);
		for (size_t i = 1; i <= resources[typeid(T)].size(); i++) {
			if (i == resources[typeid(T)].size()) resources[typeid(T)].push_back(nullptr);
			if (resources[typeid(T)][i] == nullptr) {
				resource->setId(i);
				resources[typeid(T)][i] = resource;
				break;
			}
		}
		return std::dynamic_pointer_cast<T>(resource);
	}

	template <typename T>
	const std::vector<std::shared_ptr<T>>& getResources() { return resources[typeid(T)]; }


	void update() {
		// update all internals
	}

	void show() {
		ImGui::PushID(this);
		ImGui::Begin("Resource manager");
		ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
		for (const auto& [type, typeName] : typeNames) {
			ImGui::PushID(type.name());
			if (ImGui::BeginTabItem((typeName + "s").c_str())) {
				for (const auto &resource: resources[type]) {
					if (resource == nullptr) continue;
					ImGui::PushID(&resource);
					std::string label = resource->getName() + "." + resource->getExtension();
					ImGui::Button(label.c_str(), ImVec2(50.0f, 50.0f));
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						std::tuple<std::type_index, std::shared_ptr<Resource>> payload = {type, resource};
						ImGui::SetDragDropPayload("Resource", &payload, sizeof(payload));
						ImGui::Text("Put it into render component: %s", label.c_str());
						ImGui::EndDragDropSource();
					}
					ImGui::SameLine();
					ImGui::PopID();
				}
				ImGui::EndTabItem();
			}
			ImGui::PopID();
		}
		ImGui::EndTabBar();
		ImGui::End();
		ImGui::PopID();
	}
};




/*--------------*/
/*  Components  */
/*--------------*/



class Component {
protected:
	int id;
	std::string name;
	bool active = false;
	std::shared_ptr<Entity> entity = nullptr; // owner

public:
	virtual ~Component() = default;
	void setId(int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setActive(bool active) { this->active = active; }
	void setEntity(std::shared_ptr<Entity> entity) { this->entity = entity; active = (entity != nullptr); }
	int getId() { return id; }
	const std::string& getName() { return name; }
	bool isActive() { return active; }
	std::shared_ptr<Entity> getEntity() { return entity; }
	virtual void update() = 0;
	virtual void show() = 0;
};



	std::unordered_map<std::string, int> attributePropertyLocation = {
		{"POSITION",    0}, {"NORMAL",   1}, {"TANGENT",   2},
		{"TEXCOORD_0",  3}, {"COLOR_0",  4}, {"JOINTS_0",  5}, {"WEIGHTS_0",  6},
		{"TEXCOORD_1",  7}, {"COLOR_1",  8}, {"JOINTS_1",  9}, {"WEIGHTS_1", 10},
		{"TEXCOORD_2", 11}, {"COLOR_2", 12}, {"JOINTS_2", 13}, {"WEIGHTS_2", 14},
		{"TEXCOORD_3", 15}, {"COLOR_3", 16}, {"JOINTS_3", 17}, {"WEIGHTS_3", 18}
	};

class RenderComponent : public Component {
public:
	bool alphaBlend = false;
	bool zTest = true;


private:
	std::shared_ptr<ModelResource> modelResource;
	std::unordered_map<std::string, std::shared_ptr<ShaderResource>> defaultShaderResource; // indexed by extensions.
	std::vector<std::unordered_map<std::string, std::shared_ptr<ShaderResource>>> primitiveShaderResources; // indexed by primitive index and extension

	std::shared_ptr<Component> transformComponent;

	struct PrimitiveState {
		GLuint program = 0;
		GLuint vertexArrayObject = 0;
		std::array<GLuint, 5> textureSamplers = {0, 0, 0, 0, 0};
		std::array<float, 8> textureFactors = {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f};
		int mode = 0;
		int count = 0;
		int type = 0;
		void* byteOffset = nullptr;
	};
	std::vector<PrimitiveState> primitiveStates; // indexed by primitive index
	GLuint defaultProgram = 0;


public:
	~RenderComponent() override {
		for (auto& primitiveState : primitiveStates) {
			glDeleteVertexArrays(1, &primitiveState.vertexArrayObject);
			if (primitiveState.program != 0)
				glDeleteProgram(primitiveState.program);
		}
		glDeleteProgram(defaultProgram);
	}

	void setModelResource(std::shared_ptr<ModelResource> modelResource) {
		this->modelResource = modelResource;
		setPrimitiveStates();
	}
	void setDefaultShaderResource(std::shared_ptr<ShaderResource> shader, const std::string& extension) {
		defaultShaderResource[extension] = shader;
		setDefaultProgram();
	}

	void setTransformComponent(std::shared_ptr<Component> transformComponent) { this->transformComponent = transformComponent; }

	void setPrimitiveStates() {
		if (modelResource != nullptr) {
			const auto& model = modelResource->getModel();
			const auto& bufferObjects = modelResource->getBufferObjects();
			const auto& textureObjects = modelResource->getTextureObjects();
			for (const auto& mesh: model.meshes)
				for (const auto& primitive: mesh.primitives) {
					PrimitiveState primitiveState;
					// primitiveState.program is nullptr if no program is assigned

					GLuint vertexArrayObject;
					glGenVertexArrays(1, &vertexArrayObject);
					glBindVertexArray(vertexArrayObject);
					for (const auto &attribute: primitive.attributes) {// configure vertex buffers at each location in vertex arrays
						const auto &accessor = model.accessors[attribute.second];
						const auto &bufferView = model.bufferViews[accessor.bufferView];
						glBindBuffer(GL_ARRAY_BUFFER, bufferObjects[bufferView.buffer]);
						glVertexAttribPointer(attributePropertyLocation[attribute.first], accessor.type,
											  accessor.componentType, accessor.normalized ? GL_TRUE : GL_FALSE,
											  accessor.ByteStride(bufferView),
											  (const GLvoid *) (accessor.byteOffset + bufferView.byteOffset));
						glEnableVertexAttribArray(attributePropertyLocation[attribute.first]);
						glBindBuffer(GL_ARRAY_BUFFER, 0);
					}
					const auto& accessor = model.accessors[primitive.indices];
					const auto& bufferView = model.bufferViews[accessor.bufferView];
					glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferObjects[bufferView.buffer]);// bind index buffers in vertex arrays
					glBindVertexArray(0);
					primitiveState.vertexArrayObject = vertexArrayObject;

					const auto& material = model.materials[primitive.material];
					primitiveState.textureSamplers = {
						material.pbrMetallicRoughness.baseColorTexture.index >= 0 ? textureObjects[material.pbrMetallicRoughness.baseColorTexture.index] : 0,
						material.pbrMetallicRoughness.metallicRoughnessTexture.index >= 0 ? textureObjects[material.pbrMetallicRoughness.metallicRoughnessTexture.index] : 0,
						material.normalTexture.index >= 0 ? textureObjects[material.normalTexture.index] : 0,
						material.occlusionTexture.index >= 0 ? textureObjects[material.occlusionTexture.index] : 0,
						material.emissiveTexture.index >= 0 ? textureObjects[material.emissiveTexture.index] : 0};
					primitiveState.textureFactors = {
						material.pbrMetallicRoughness.baseColorFactor.size() == 4 ? (float) material.pbrMetallicRoughness.baseColorFactor[0] : 1.0f,
						material.pbrMetallicRoughness.baseColorFactor.size() == 4 ? (float) material.pbrMetallicRoughness.baseColorFactor[1] : 1.0f,
						material.pbrMetallicRoughness.baseColorFactor.size() == 4 ? (float) material.pbrMetallicRoughness.baseColorFactor[2] : 1.0f,
						material.pbrMetallicRoughness.metallicFactor >= 0 ? (float) material.pbrMetallicRoughness.metallicFactor : 1.0f,
						material.pbrMetallicRoughness.roughnessFactor >= 0 ? (float) material.pbrMetallicRoughness.roughnessFactor : 1.0f,
						material.emissiveFactor.size() == 3 ? (float) material.emissiveFactor[0] : 0.0f,
						material.emissiveFactor.size() == 3 ? (float) material.emissiveFactor[1] : 0.0f,
						material.emissiveFactor.size() == 3 ? (float) material.emissiveFactor[2] : 0.0f};

					primitiveState.mode = primitive.mode;
					primitiveState.count = accessor.count;
					primitiveState.type = accessor.componentType;
					primitiveState.byteOffset = (void *) (accessor.byteOffset + bufferView.byteOffset);

					primitiveStates.push_back(primitiveState);
				}
		}
	}

	void setDefaultProgram() {
		if (defaultShaderResource["vert"] != nullptr && defaultShaderResource["frag"] != nullptr) {
			auto defaultProgram = glCreateProgram();
			glAttachShader(defaultProgram, defaultShaderResource["vert"]->getShader());
			glAttachShader(defaultProgram, defaultShaderResource["frag"]->getShader());
			glLinkProgram(defaultProgram);
			int success;
			glGetProgramiv(defaultProgram, GL_LINK_STATUS, &success);
			if (!success) {
				char infoLog[512];
				glGetProgramInfoLog(defaultProgram, 512, NULL, infoLog);
				std::cerr << "Error: Failed to link shaders" << infoLog << std::endl;
			}
			this->defaultProgram = defaultProgram;
		}
	}

	std::shared_ptr<ModelResource> getModelResource() { return modelResource; }
	std::vector<PrimitiveState>& getPrimitiveStates() { return primitiveStates; }
	GLuint getDefaultProgram() { return defaultProgram; }
	std::shared_ptr<Component> getTransformComponent() { return transformComponent; }




	void update() override {
		// update all internals
	}

	void show() override { // show in entity manager window.
		ImGui::Checkbox("Active", &active);
		ImGui::Text("Model: %s", modelResource ? modelResource->getName().c_str() : "None");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource")) {
				auto data = (std::pair<std::type_index, std::shared_ptr<Resource>>*) payload->Data;
				auto type = (*data).first;
				auto resource = (*data).second;
				if (type == typeid(ModelResource))
					setModelResource(std::dynamic_pointer_cast<ModelResource>(resource));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Text("Vertex shader: %s", defaultShaderResource["vert"] ? defaultShaderResource["vert"]->getName().c_str() : "None");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource")) {
				auto data = (std::pair<std::type_index, std::shared_ptr<Resource>>*) payload->Data;
				auto type = (*data).first;
				auto resource = (*data).second;
				if (type == typeid(ShaderResource) && resource->getExtension() == "vert")
					setDefaultShaderResource(std::dynamic_pointer_cast<ShaderResource>(resource), "vert");
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Text("Fragment shader: %s", defaultShaderResource["frag"] ? defaultShaderResource["frag"]->getName().c_str() : "None");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Resource")) {
				auto data = (std::pair<std::type_index, std::shared_ptr<Resource>>*) payload->Data;
				auto type = (*data).first;
				auto resource = (*data).second;
				if (type == typeid(ShaderResource) && resource->getExtension() == "frag")
					setDefaultShaderResource(std::dynamic_pointer_cast<ShaderResource>(resource), "frag");
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		if (ImGui::TreeNode("Options")) {
			ImGui::Checkbox("Alpha blend", &alphaBlend);
			ImGui::Checkbox("Z test", &zTest);
			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Textures")) {
			ImGui::Text("Primitive count: %d", (int) primitiveStates.size());
			static ImGuiTableFlags flags = ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders |
										   ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY;
			static float textHeight = ImGui::GetTextLineHeightWithSpacing();
			if (ImGui::BeginTable("Textures", 14, flags, ImVec2(0.0f, textHeight * 6))) {
				ImGui::TableSetupColumn("Primitive");
				ImGui::TableSetupColumn("BaseColor");
				ImGui::TableSetupColumn("MetallicRoughness");
				ImGui::TableSetupColumn("Normal");
				ImGui::TableSetupColumn("Occlusion");
				ImGui::TableSetupColumn("Emmisive");
				ImGui::TableSetupColumn("BaseColorFactor");
				ImGui::TableSetupColumn("BaseColorFactor");
				ImGui::TableSetupColumn("BaseColorFactor");
				ImGui::TableSetupColumn("MetallicFactor");
				ImGui::TableSetupColumn("RoughnessFactor");
				ImGui::TableSetupColumn("EmmisiveFactor");
				ImGui::TableSetupColumn("EmmisiveFactor");
				ImGui::TableSetupColumn("EmmisiveFactor");
				ImGui::TableHeadersRow();
				for (auto& primitiveState: primitiveStates) {
					ImGui::PushID(&primitiveState);
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
					ImGui::Text("%d", ImGui::TableGetRowIndex());
					for (int column = 1; column < 6; column++) {
						ImGui::PushID(column);
						ImGui::TableNextColumn();
						ImGui::InputScalar("", ImGuiDataType_U32, &primitiveState.textureSamplers[column - 1]);
						ImGui::PopID();
					}
					for (int column = 6; column < 14; column++) {
						ImGui::PushID(column);
						ImGui::TableNextColumn();
						ImGui::InputScalar("", ImGuiDataType_Float, &primitiveState.textureFactors[column - 6]);
						ImGui::PopID();
					}
					ImGui::PopID();
				}
				ImGui::EndTable();
			}
			ImGui::TreePop();
		}
	}
};



class TransformComponent : public Component {
public:
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec4 scale = glm::vec4(1.0f);

private:
    glm::mat4 modelMatrix = glm::mat4(1.0f);

public:
	void setPosition(const glm::vec3& position) { this->position = position; setModelMatrix(); }
	void setRotation(const glm::vec3& rotation) { this->rotation = rotation; setModelMatrix(); }
	void setScale(const glm::vec4& scale) { this->scale = scale; setModelMatrix();}

	void setModelMatrix() {
        modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::translate(modelMatrix, position);
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(scale) * scale.w);
    }

    const glm::mat4& getModelMatrix() { return modelMatrix; }

    void update() override {
        // update all internals (not by ImGui)
    }

	void show() override {
		ImGui::Checkbox("Active", &active);
		if (ImGui::DragFloat3("Position", (float*) &position, 0.1f, -3.40282e+38, 3.40282e+38))
			setModelMatrix();
		if (ImGui::SliderFloat3("Rotation", (float*) &rotation, -180.0f, 180.0f))
			setModelMatrix();
		if (ImGui::DragFloat4("Scale", (float*) &scale, 0.1f, 0.0f, 3.40282e+38))
			setModelMatrix();
	}
};


class CameraComponent : public Component {
private:
	glm::vec3 position, front, up, right, worldUp;
	float yaw, pitch;
	float movementSpeed, mouseSensitivity;
	float zoom;

	glm::mat4 viewMatrix;

public:
	CameraComponent() : position(0.0f), front(0.0f, 0.0f, -1.0f), up(0.0f, 1.0f, 0.0f), right(1.0f, 0.0f, 0.0f),
			   worldUp(0.0f, 1.0f, 0.0f), yaw(-90.0f), pitch(0.0f), movementSpeed(2.5f), mouseSensitivity(0.1f), zoom(45.0f)
	{ updateCameraVectors(); updateViewMatrix(); }

	const glm::mat4& getViewMatrix() { return viewMatrix; }

	void update() override {}

	void updateCameraVectors() {
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(front);
		right = glm::normalize(glm::cross(front, worldUp));
		up = glm::normalize(glm::cross(right, front));
	}

	void updateViewMatrix() {
		viewMatrix = glm::lookAt(position, position + front, up);
	}

	void show() override {}
};



class LightComponent : public Component {
private:
	glm::vec3 position, color;
	float intensity;
public:
	LightComponent() : position(0.0f), color(1.0f), intensity(1.0f) {}


	void update() override {
		// update all internals
	}

	void show() override {}
};




class ComponentManager {
private:
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<Component>>> components; // indexed by type and id

	std::unordered_map<std::type_index, std::string> typeNames
		= {{typeid(RenderComponent), "Render"},
		   {typeid(TransformComponent), "Transform"},
		   {typeid(CameraComponent), "Camera"},
		   {typeid(LightComponent), "Light"}};

public:
	template <typename T>
	std::shared_ptr<T> addComponent() {
		std::shared_ptr<Component> component = std::make_shared<T>();
		if (components[typeid(T)].empty()) components[typeid(T)].push_back(nullptr);
		for (size_t i = 1; i <= components[typeid(T)].size(); i++) {
			if (i == components[typeid(T)].size()) components[typeid(T)].push_back(nullptr);
			if (components[typeid(T)][i] == nullptr) {
				component->setId(i);
				components[typeid(T)][i] = component;
				break;
			}
		}
		component->setName(typeNames[typeid(T)] + " " + std::to_string(component->getId()));
		return std::dynamic_pointer_cast<T>(component);
	}


	const std::vector<std::shared_ptr<Component>>& getComponents(std::type_index type) { return components[type]; }


	void show() {
		ImGui::PushID(this);
		ImGui::Begin("Component manager");
		ImGui::BeginTabBar("Components", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
		for (const auto& [type, typeName] : typeNames) {
			if (ImGui::BeginTabItem(typeName.c_str())) {
				for (const auto &component: components[type]) {
					if (component == nullptr) continue;
					ImGui::PushID(&component);
					std::string label = component->getName();
					label += " (ID: " + std::to_string(component->getId());
					if (component->getEntity())
						label += ", owner ID: "; // std::to_string(component->getEntity()->getId());
					label += ")";
					bool open = ImGui::CollapsingHeader(label.c_str());
					if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
						std::tuple<std::type_index, std::shared_ptr<Component>> payload = {type, component};
						ImGui::SetDragDropPayload("Component", &payload, sizeof(payload));
						ImGui::Text("Put it into entity: %s", component->getName().c_str());
						ImGui::EndDragDropSource();
					}
					if (open) {
						ImGui::Indent();
						component->show();
						ImGui::Unindent();
					}
					ImGui::PopID();
				}
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
		ImGui::End();
		ImGui::PopID();
	}

};





/*------------*/
/*  Entities  */
/*------------*/



class Entity : public std::enable_shared_from_this<Entity> {
private:
	int id;
	std::string name;
	bool active = true;
	std::unordered_map<std::type_index, std::unique_ptr<Component>> component;

	int _id;
	std::string _name;
	bool _active = true;
	std::unordered_map<std::type_index, std::unique_ptr<Component>> _component;
	std::vector<std::unique_ptr<Entity>> _children;
    Entity* _parent = nullptr;

public:

	void setId(int id) { this->id = id; }
	void setName(const std::string& name) { this->name = name; }
	void setComponent(std::unique_ptr<Component> component, std::type_index type) {
		// TODO: ???
		this->component[type] = std::move(component);
		if (component == nullptr) return;
		if (component->getEntity() != nullptr) {
			component->getEntity()->getComponent(type)->setActive(false);
			component->getEntity()->getComponent(type)->setEntity(nullptr);
			component->getEntity()->setComponent(nullptr, type);
		}
		component->setEntity(shared_from_this());


		if (type == typeid(RenderComponent) || type == typeid(TransformComponent)) {
			if (this->component[typeid(RenderComponent)] != nullptr && this->component[typeid(TransformComponent)] != nullptr)
				std::dynamic_pointer_cast<RenderComponent>(this->component[typeid(RenderComponent)])
					->setTransformComponent(this->component[typeid(TransformComponent)]);
			else if (this->component[typeid(RenderComponent)] != nullptr)
				std::dynamic_pointer_cast<RenderComponent>(this->component[typeid(RenderComponent)])->setTransformComponent(nullptr);
		}
	}
	template <typename T>
	void setComponent(std::shared_ptr<T> component) { setComponent(component, typeid(T)); }

	int getId() const { return id; }
	const std::string& getName() const { return name; }
	std::shared_ptr<Component> getComponent(std::type_index type) { return component[type]; }
	const std::unordered_map<std::type_index, std::shared_ptr<Component>>& getComponents() { return component; }


	void update() {
		for (const auto& [type, component] : component)
			component->update();
	}

	void show() {
		ImGui::Checkbox("Active", &active);
		for (const auto& [type, component] : component)
			if (component != nullptr) {
				ImGui::PushID(&component);
				ImGui::SeparatorText((component->getName() + " (ID: " + std::to_string(component->getId()) + ")").c_str());
				ImGui::Indent();
				component->show();
				ImGui::Unindent();
				ImGui::PopID();
			}
		ImGui::Text("Drag and drop to set components.");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Component")) {
				auto data = (std::pair<std::type_index, std::shared_ptr<Component>>*) payload->Data;
				auto type = (*data).first;
				auto component = (*data).second;
				setComponent(component, type);
			}
			ImGui::EndDragDropTarget();
		}
	}


};



class EntityManager {
private:
	std::vector<std::shared_ptr<Entity>> entities; // indexed by id



public:
	std::shared_ptr<Entity> addEntity() {
		const auto& entity = std::make_shared<Entity>();
		if (entities.empty()) entities.push_back(nullptr);
		for (size_t i = 1; i <= entities.size(); i++) {
			if (i == entities.size()) entities.push_back(nullptr);
			if (entities[i] == nullptr) {
				entity->setId(i);
				entities[i] = entity;
				break;
			}
		}
		entity->setName("Entity " + std::to_string(entity->getId()));
		return entity;
	}

	std::vector<std::shared_ptr<Entity>> getEntities() { return entities; }


	void update() {
		for (const auto& entity : entities)
			entity->update();
	}

	void show() {
		ImGui::PushID(this);
		ImGui::Begin("Entity manager"); // TODO: reorderable.
		for (const auto& entity : entities) {
			if (entity == nullptr) continue;
			ImGui::PushID(&entity);
			ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			bool open = ImGui::CollapsingHeader((entity->getName() + " (ID: " + std::to_string(entity->getId()) + ")").c_str());
			std::string menuAction = "";
			if (ImGui::BeginPopupContextItem()) {
				if (ImGui::MenuItem("Edit name")) menuAction = "Edit name";
				if (ImGui::MenuItem("Remove")) menuAction = "Remove";
				ImGui::EndPopup();
			}
			if (menuAction == "Edit name") ImGui::OpenPopup("Edit name");
			else if (menuAction == "Remove") ImGui::OpenPopup("Remove?");

			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Edit name", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
				static std::string name;
				ImGui::InputText("Name", &name);
				if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0)) ||
					(ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused()))
					ImGui::CloseCurrentPopup(), entity->setName(name), name = "";
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(-1, 0)) ||
					(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
			if (ImGui::BeginPopupModal("Remove?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Are you sure you want to remove this entity?");
				if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0)) ||
					(ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
					ImGui::CloseCurrentPopup();
					for (const auto& [type, component] : entity->getComponents())
						if (component != nullptr) {
							component->setActive(false);
							component->setEntity(nullptr);
						}
					entities[entity->getId()] = nullptr;
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(-1, 0)) ||
					(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
					ImGui::CloseCurrentPopup();
				ImGui::EndPopup();
			}

			if (open && entity != nullptr) entity->show();
			ImGui::PopID();
		}

		if (ImGui::Button("Add entity", ImVec2(-1, 0)))
			ImGui::OpenPopup("Add entity");
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		if (ImGui::BeginPopupModal("Add entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
			static std::string name;
			if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
			ImGui::InputText("Name", &name);
			if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0))
					|| (ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
				ImGui::CloseCurrentPopup();
				if (name == "") addEntity();
				else { addEntity()->setName(name); name = ""; }
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(-1, 0))
					|| (ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
		ImGui::End();
		ImGui::PopID();
	}



};




/*-----------*/
/*  Systems  */
/*-----------*/



// These data should be saved in camera and light classes, and passed to draw method.
// these variables should be dynamically defined per cameras.

glm::mat4 viewMatrix = glm::lookAt(glm::vec3(0, 0, -10), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), 1280.0f / 960.0f, 0.1f, 100.0f);
glm::vec3 cameraPosition = glm::vec3(0.0f);

// these variables should be dynamically defined per lights
glm::vec3 lightPosition = glm::vec3(1.0f, 3.0f, -3.0f);
glm::vec3 lightColor = glm::vec3(0.8f, 0.9f, 1.0f);



class RenderSystem {
private:
	ComponentManager* componentManager;


	glm::vec4 clearColor = glm::vec4(0.45f, 0.55f, 0.60f, 1.00f);

public:
	void setComponentManager(ComponentManager* componentManager) { this->componentManager = componentManager; }



	void update() {}

	void render() {

		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLuint previousProgram = 0;
		std::array<GLuint, 5> previousTextureSamplers = {0, 0, 0, 0, 0};
		std::array<float, 8> previousTextureFactors = {};


		for (const auto& component : componentManager->getComponents(typeid(RenderComponent))) {
			const auto& renderComponent = std::dynamic_pointer_cast<RenderComponent>(component);

			if (!renderComponent) continue;
			if (!renderComponent->isActive()) continue;
			if (!renderComponent->getModelResource()) continue;

			glm::mat4 modelMatrix(1.0f);
			const auto& entity = renderComponent->getEntity();
			if (const auto &transformComponent = entity->getComponent(typeid(TransformComponent)))
				modelMatrix = std::dynamic_pointer_cast<TransformComponent>(transformComponent)->getModelMatrix();



			const auto& primitiveStates = renderComponent->getPrimitiveStates();
			for (const auto& primitiveState : primitiveStates) {
				const auto& currentProgram = primitiveState.program != 0 ? primitiveState.program : renderComponent->getDefaultProgram();
				const auto& currentTextureSamplers = primitiveState.textureSamplers;
				const auto& currentTextureFactors = primitiveState.textureFactors;

				if (currentProgram != previousProgram) {
					glUseProgram(currentProgram);

					// uniforms
					// independent of primitives and shaders but should be called if program is changed
					glUniformMatrix4fv(glGetUniformLocation(currentProgram, "uModel"), 1, GL_FALSE,
									   &modelMatrix[0][0]);
					glUniformMatrix4fv(glGetUniformLocation(currentProgram, "uView"), 1, GL_FALSE,
									   &viewMatrix[0][0]);
					glUniformMatrix4fv(glGetUniformLocation(currentProgram, "uProjection"), 1, GL_FALSE,
									   &projectionMatrix[0][0]);
					glUniform3fv(glGetUniformLocation(currentProgram, "uCameraPosition"), 1,
								 &cameraPosition[0]);
					glUniform3fv(glGetUniformLocation(currentProgram, "uLightPosition"), 1,
								 &lightPosition[0]);
					glUniform3fv(glGetUniformLocation(currentProgram, "uLightColor"), 1,
								 &lightColor[0]);
				}

				// texture samplers and factors
				// clearly depend on both primitives and programs
				for (size_t i = 0; i < 5; i++)
					if (currentTextureSamplers[i] != previousTextureSamplers[i] || currentProgram != previousProgram) {
						glActiveTexture(GL_TEXTURE0 + i);
						glBindTexture(GL_TEXTURE_2D, currentTextureSamplers[i]);

						if (i == 0) glUniform1i(glGetUniformLocation(currentProgram, "tBaseColorTexture"), 0);
						if (i == 1) glUniform1i(glGetUniformLocation(currentProgram, "tMetallicRoughnessTexture"), 1);
						if (i == 2) glUniform1i(glGetUniformLocation(currentProgram, "tNormalTexture"), 2);
						if (i == 3) glUniform1i(glGetUniformLocation(currentProgram, "tOcclusionTexture"), 3);
						if (i == 4) glUniform1i(glGetUniformLocation(currentProgram, "tEmissiveTexture"), 4);
					}
				if (currentTextureFactors[0] != previousTextureFactors[0] ||
					currentTextureFactors[1] != previousTextureFactors[1] ||
					currentTextureFactors[2] != previousTextureFactors[2] || currentProgram != previousProgram)
					glUniform3f(glGetUniformLocation(currentProgram, "tBaseColorFactor"),
								primitiveState.textureFactors[0],
								primitiveState.textureFactors[1],
								primitiveState.textureFactors[2]);
				if (currentTextureFactors[3] != previousTextureFactors[3] || currentProgram != previousProgram)
					glUniform1f(glGetUniformLocation(currentProgram, "tMetallicFactor"),
								primitiveState.textureFactors[3]);
				if (currentTextureFactors[4] != previousTextureFactors[4] || currentProgram != previousProgram)
					glUniform1f(glGetUniformLocation(currentProgram, "tRoughnessFactor"),
								primitiveState.textureFactors[4]);
				if (currentTextureFactors[5] != previousTextureFactors[5] ||
					currentTextureFactors[6] != previousTextureFactors[6] ||
					currentTextureFactors[7] != previousTextureFactors[7] || currentProgram != previousProgram)
					glUniform3f(glGetUniformLocation(currentProgram, "tEmissiveFactor"),
								primitiveState.textureFactors[5],
								primitiveState.textureFactors[6],
								primitiveState.textureFactors[7]);

				glBindVertexArray(primitiveState.vertexArrayObject);
				glDrawElements(primitiveState.mode, primitiveState.count, primitiveState.type, primitiveState.byteOffset);
				glBindVertexArray(0);

				previousProgram = currentProgram;
				previousTextureSamplers = currentTextureSamplers;
				previousTextureFactors = currentTextureFactors;
			}
		}
	}


};






/*-----------*/
/*  Windows  */
/*-----------*/


class Window {
private:
	std::string name;
	int width, height;
	GLFWwindow* window;

	bool showDemoWindow = false;

	ResourceManager* resourceManager;
	ComponentManager* componentManager;
	EntityManager* entityManager;

public:
	Window(std::string name, int width, int height) : name(std::move(name)), width(width), height(height) {}

	void setName(const std::string& name) { this->name = name; }
	void setWidth(int width) { this->width = width; }
	void setHeight(int height) { this->height = height; }
	void setResourceManager(ResourceManager* resourceManager) { this->resourceManager = resourceManager; }
	void setComponentManager(ComponentManager* componentManager) { this->componentManager = componentManager; }
	void setEntityManager(EntityManager* entityManager) { this->entityManager = entityManager; }


	void initialize() {
		if (!glfwInit()) std::cerr << "Error: Failed to initialize GLFW" << std::endl;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	#ifdef __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	#endif
		window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);
		if (!window) std::cerr << "Error: Failed to create GLFW window" << std::endl;
		glfwMakeContextCurrent(window);

		glewInit();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");


		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_TEST);
	}

	void terminate() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();
	}

	bool shouldClose() {
		return glfwWindowShouldClose(window);
	}
	void pollEvents() {
		glfwPollEvents();
	}
	void swapBuffers() {
		glfwSwapBuffers(window);
	}

	void show() {
		ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();
		ImGui::Begin("Hello, world!");
		ImGui::Checkbox("Demo Window", &showDemoWindow);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
					1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();

		if (showDemoWindow)
			ImGui::ShowDemoWindow(&showDemoWindow);

		resourceManager->show();
		entityManager->show();
		componentManager->show();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

};











}
#endif //GLTF_VIEWER_IKHANCHOI_H
