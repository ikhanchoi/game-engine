#include <fstream>
#include <iostream>
#include <sstream>

#include "resource.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

namespace ikhanchoi {

static const std::unordered_map<std::type_index, std::string> resourceTypeNames
	= {{typeid(ModelResource), "Model"},
	   {typeid(ShaderResource), "Shader"}};

ModelResource::~ModelResource() {
	for (const auto &bufferObject: bufferObjects) {
		glDeleteBuffers(1, &bufferObject);
	}
	for (const auto &textureObject: textureObjects) {
		glDeleteTextures(1, &textureObject);
	}
}

void ModelResource::loadModel() {
	std::string err, warn;
	bool ret = false;
	tinygltf::TinyGLTF loader;

	std::string extension = name.substr(name.rfind('.') + 1);
	if (extension == "gltf")
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, "../assets/models/" + name);
	else if (extension == "glb")
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, "../assets/models/" + name);
	else std::cerr << "Error: (loadModel) Failed to parse model extension. " << extension << '\n';

	if (!warn.empty()) std::cout << "Warning: (loadModel) " << warn << '\n';
	if (!err.empty())  std::cerr << "Error: (loadModel) " << err << '\n';
	if (!ret)          std::cerr << "Error: (loadModel) Failed to parse gltf file." << '\n';
}

void ModelResource::loadBufferObjects() {
	for (const auto &buffer: model.buffers) {
		GLuint bufferObject;
		glGenBuffers(1, &bufferObject);
		glBindBuffer(GL_ARRAY_BUFFER, bufferObject);
		glBufferData(GL_ARRAY_BUFFER, buffer.data.size(), buffer.data.data(), GL_STATIC_DRAW);
		bufferObjects.push_back(bufferObject);
	}
}

void ModelResource::loadTextureObjects() {
	for (const auto &image: model.images) {
		GLuint textureObject;
		glGenTextures(1, &textureObject);
		glBindTexture(GL_TEXTURE_2D, textureObject);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		textureObjects.push_back(textureObject);
	}
}

void ModelResource::show() {
	ImGui::PushID(this);
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool open = ImGui::CollapsingHeader((name + " (id: " + std::to_string(id) + ")").c_str());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		std::tuple<std::type_index, ModelResource*> payload = {type, this};
		ImGui::SetDragDropPayload("ModelResource", &payload, sizeof(payload));
		ImGui::Text("Put it into render component: %s", name.c_str());
		ImGui::EndDragDropSource();
	}
	if (open) { // TODO: add more information for each buffer and texture objects.
		ImGui::Checkbox("active", &active);
		ImGui::Text("number of buffer objects: %d", (int) bufferObjects.size());
		ImGui::Text("number of texture objects: %d", (int) textureObjects.size());
	}
	ImGui::PopID();
}


ShaderResource::~ShaderResource() {
	glDeleteShader(shader);
}

void ShaderResource::loadShader() {
	std::ifstream fs;
	std::string code;
	const char* cstr;
	int success;

	fs.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	try {
		fs.open("../assets/shaders/" + name);
		std::stringstream ss;
		ss << fs.rdbuf();
		code = ss.str();
		fs.close();
	} catch (std::ifstream::failure& err) {
		std::cerr << "Error: (loadShader) Failed to load shader. " << err.what() << "\n";
	}
	cstr = code.c_str();

	std::string extension = name.substr(name.rfind('.') + 1);
	if (extension == "vert")
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (extension == "frag")
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else std::cerr << "Error: (loadShader) Failed to parse shader extension. " << extension << '\n';

	glShaderSource(shader, 1, &cstr, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cerr << "Error: (loadShader) Failed to compile shader. " << infoLog << '\n';
	}
}

void ShaderResource::show() {
	ImGui::PushID(this);
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool open = ImGui::CollapsingHeader((name + " (id: " + std::to_string(id) + ")").c_str());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		std::tuple<std::type_index, ShaderResource*> payload = {type, this};
		ImGui::SetDragDropPayload("ShaderResource", &payload, sizeof(payload));
		ImGui::Text("Put it into render component: %s", name.c_str());
		ImGui::EndDragDropSource();
	}
	if (open) {
		ImGui::Checkbox("active", &active);
		ImGui::Text("shader id: %d", shader);
	}
	ImGui::PopID();
}




void ResourceManager::show() {
	ImGui::PushID(this);
	ImGui::Begin("Resource manager");
	ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
	for (const auto& [type, typeName] : resourceTypeNames) {
		ImGui::PushID(type.name());
		if (ImGui::BeginTabItem((typeName + "s").c_str())) {
			for (const auto &resource: resources[type]) {
				if (resource == nullptr)
					continue;
				else
					resource->show();
			}
			ImGui::EndTabItem();
		}
		ImGui::PopID();
	}
	ImGui::EndTabBar();
	ImGui::End();
	ImGui::PopID();
}

}