#include "window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "modules/entity.h"
#include "modules/event.h"
#include "modules/resource.h"

namespace ikhanchoi {


/*----------*/
/* Resource */
/*----------*/

void WindowUpdater::update(ModelResource& modelResource) {
	ImGui::PushID(&modelResource);
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool open = ImGui::CollapsingHeader((modelResource.getName() + " (id: " + std::to_string(modelResource.getId()) + ")").c_str());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		std::tuple<std::type_index, ModelResource*> payload = {typeid(ModelResource), &modelResource};
		ImGui::SetDragDropPayload("ModelResource", &payload, sizeof(payload));
		ImGui::Text("Put it into render component: %s", modelResource.getName().c_str());
		ImGui::EndDragDropSource();
	}
	if (open) {// TODO: show more information for each buffer and texture objects.
		ImGui::Checkbox("active", modelResource.accessActive());
		ImGui::Text("Inactive means that this resource is unloaded.");
		ImGui::Text("number of buffer objects: %d", (int) modelResource.getBufferObjects().size());
		ImGui::Text("number of texture objects: %d", (int) modelResource.getTextureObjects().size());
	}
	ImGui::PopID();
}

void WindowUpdater::update(ShaderResource& shaderResource) {
	ImGui::PushID(&shaderResource);
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool open = ImGui::CollapsingHeader((shaderResource.getName() + " (id: " + std::to_string(shaderResource.getId()) + ")").c_str());
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
		std::tuple<std::type_index, ShaderResource*> payload = {typeid(ShaderResource), &shaderResource};
		ImGui::SetDragDropPayload("ShaderResource", &payload, sizeof(payload));
		ImGui::Text("Put it into render component: %s", shaderResource.getName().c_str());
		ImGui::EndDragDropSource();
	}
	if (open) {
		ImGui::Checkbox("active", shaderResource.accessActive());
		ImGui::Text("Inactive means that this resource is unloaded.");
		ImGui::Text("shader id: %d", shaderResource.getShader());
	}
	ImGui::PopID();
}

void WindowUpdater::update(ResourceManager& resourceManager) {
	ImGui::PushID(&resourceManager);
	ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
	for (const auto &[resourceType, resourceTypeName]: resourceManager.getTypes()) {
		ImGui::PushID(resourceType.name());
		if (ImGui::BeginTabItem((resourceTypeName + "s").c_str())) {
			resourceManager.forEach<Resource>(resourceType, [this](Resource* resource) {
				if (resource->isActive())
					resource->update(*this);
			});
			ImGui::EndTabItem();
		}
		ImGui::PopID();
	}
	ImGui::EndTabBar();
	ImGui::PopID();
}



/*-----------*/
/* Component */
/*-----------*/


/*--------*/
/* Entity */
/*--------*/

void WindowUpdater::update(Entity& entity) {
	ImGui::PushID(&entity);
	ImGui::SetNextItemOpen(true, ImGuiCond_Once);
	bool open = ImGui::CollapsingHeader((entity.getName() + " (ID: " + std::to_string(entity.getId()) + ")").c_str());
	std::string menu;
	if (ImGui::BeginPopupContextItem()) {
		if (ImGui::MenuItem("Edit name"))
			menu = "Edit name";
		if (ImGui::MenuItem("Remove"))
			menu = "Remove";
		ImGui::EndPopup();
	}
	if (menu == "Edit name")
		ImGui::OpenPopup("Edit name");
	else if (menu == "Remove")
		ImGui::OpenPopup("Remove?");

	if (open) {
		ImGui::Checkbox("Active", entity.accessActive());
		for (const auto& [componentType, component] : entity.getComponent())
			component->update(*this);
		ImGui::Text("Drag and drop to set components.");
		if (ImGui::BeginDragDropTarget()) {
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Component")) {
				auto data = (std::pair<std::type_index, std::shared_ptr<Component>>*) payload->Data;
				auto componentType = (*data).first;
				auto component = (*data).second;
				// set component event?
			}
			ImGui::EndDragDropTarget();
		}
	}

	// Edit name popup
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Edit name", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		if (ImGui::IsWindowAppearing())
			ImGui::SetKeyboardFocusHere();
		std::string name;
		ImGui::InputText("Name", &name);
		if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0)) ||
			(ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
			ImGui::CloseCurrentPopup();
			entity.setName(name); // edit name event?
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(-1, 0)) ||
			(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	// Remove popup
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Remove?", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		ImGui::Text("Are you sure you want to remove this entity?");
		if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0)) ||
			(ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
			ImGui::CloseCurrentPopup();
			// remove event?
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(-1, 0)) ||
			(ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}

	ImGui::PopID();
}


void WindowUpdater::update(EntityManager& entityManager) {
	ImGui::PushID(&entityManager); // TODO: reorderable.
	for (const auto& entity : entityManager.getEntities()) {
		if (entity == nullptr)
			continue;
		entity->update(*this);
	}

	if (ImGui::Button("Add entity", ImVec2(-1, 0)))
		ImGui::OpenPopup("Add entity");
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (ImGui::BeginPopupModal("Add entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
		std::string name;
		if (ImGui::IsWindowAppearing()) ImGui::SetKeyboardFocusHere();
		ImGui::InputText("Name", &name);
		if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f, 0))
				|| (ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
			ImGui::CloseCurrentPopup();
			entityManager.addEntity(name); // add entity event?
		}
		ImGui::SameLine();
		if (ImGui::Button("Cancel", ImVec2(-1, 0))
				|| (ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
			ImGui::CloseCurrentPopup();
		ImGui::EndPopup();
	}
	ImGui::PopID();
}


/*----------------*/
/* Window Manager */
/*----------------*/


WindowManager::WindowManager(unsigned int id, const std::string& name) : Manager(id, name) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

WindowManager::~WindowManager() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void WindowUpdater::update(Window& window) {
	ImGui::Begin(window.getName().c_str());
	std::variant<std::shared_ptr<Object>, std::shared_ptr<Manager>> content = window.getContent();
	std::visit([this](auto& content) {
	    if (content)
	        content->update(*this);
	}, content);
	ImGui::End();
}

void WindowUpdater::update(WindowManager& windowManager) {
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();
	ImGui::Begin(windowManager.getName().c_str());
	ImGui::Checkbox("Demo window", windowManager.accessDemo());
	windowManager.forEach<Window>(typeid(Window), [](Window* window) {
		ImGui::Checkbox((window->getName() + " window").c_str(), window->accessActive());
	});
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
				1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	if (*windowManager.accessDemo())
		ImGui::ShowDemoWindow();
	windowManager.forEach<Window>(typeid(Window), [this](Window* window) {
		if (window->isActive())
			window->update(*this);
	});
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}



}