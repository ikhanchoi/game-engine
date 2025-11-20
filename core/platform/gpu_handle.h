#pragma once

#include <variant> // TODO: custom variant if needed?

struct OpenGLHandle { unsigned int id = 0; };
struct VulkanHandle { void* vkModule = nullptr; }; // VkShaderModule  etc
struct DirectXHandle { void* dxPtr = nullptr; };   // ID3D12PipelineState*  etc
struct MetalHandle { void* mtlObj = nullptr; };    // MTLFunction*  etc

class GPUHandle {
	enum class API : uint8_t { OpenGL, Vulkan, DirectX, Metal } api;
	std::variant<OpenGLHandle, VulkanHandle, DirectXHandle, MetalHandle> handle;
public:
	explicit GPUHandle(OpenGLHandle handle) : api(API::OpenGL), handle(handle) {}
	explicit GPUHandle(VulkanHandle handle) : api(API::Vulkan), handle(handle) {}
	explicit GPUHandle(DirectXHandle handle) : api(API::DirectX), handle(handle) {}
	explicit GPUHandle(MetalHandle handle) : api(API::Metal), handle(handle) {}

	API getAPI() const { return api; }
	template<typename GPUHandleType>
	GPUHandleType get() const { return std::get<GPUHandleType>(handle); }
};


