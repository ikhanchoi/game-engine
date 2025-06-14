#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "resource.h"

namespace ikhanchoi {


/*---------------*/
/* ModelResource */
/*---------------*/

ModelResource::~ModelResource() {
	for (const auto &bufferObject: bufferObjects)
		glDeleteBuffers(1, &bufferObject);
	for (const auto &textureObject: textureObjects)
		glDeleteTextures(1, &textureObject);
}

void ModelResource::loadModel(const std::string& path) {
	std::string err, warn;
	bool ret = false;
	tinygltf::TinyGLTF loader;

	std::string extension = path.substr(path.rfind('.') + 1);
	if (extension == "gltf")
		ret = loader.LoadASCIIFromFile(&model, &err, &warn, path);
	else if (extension == "glb")
		ret = loader.LoadBinaryFromFile(&model, &err, &warn, path);
	else
		throw std::runtime_error("Error: (ModelResource::loadModel) Failed to parse model extension: " + extension);

	if (!warn.empty())
		throw std::runtime_error("Warning: (ModelResource::loadModel) " + warn);
	if (!err.empty())
		throw std::runtime_error("Error: (ModelResource::loadModel) " + err);
	if (!ret)
		throw std::runtime_error("Error: (ModelResource::loadModel) Failed to parse gltf file: " + path);
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




/*----------------*/
/* ShaderResource */
/*----------------*/


ShaderResource::~ShaderResource() {
	glDeleteShader(shader);
}

void ShaderResource::loadShader(const std::string& path) {
	std::ifstream fs;
	std::string code;
	const char* cstr;
	int success;

	fs.exceptions(std::ifstream::failbit|std::ifstream::badbit);
	try {
		fs.open(path);
		std::stringstream ss;
		ss << fs.rdbuf();
		code = ss.str();
		fs.close();
	} catch (std::ifstream::failure& err) {
		throw std::runtime_error("Error: (ShaderResource::loadShader) Failed to load shader file: " + path + ". " + err.what());
	}
	cstr = code.c_str();

	std::string extension = path.substr(path.rfind('.') + 1);
	if (extension == "vert")
		shader = glCreateShader(GL_VERTEX_SHADER);
	else if (extension == "frag")
		shader = glCreateShader(GL_FRAGMENT_SHADER);
	else
		throw std::runtime_error("Error: (ShaderResource::loadShader) Failed to parse shader extension: " + extension);

	glShaderSource(shader, 1, &cstr, NULL);
	glCompileShader(shader);
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success) {
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		throw std::runtime_error("Error: (ShaderResource::loadShader) Failed to compile shader: " + path + ". " + infoLog);
	}
}



/*-----------------*/
/* ResourceManager */
/*-----------------*/



}