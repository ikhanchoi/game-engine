#include "window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "misc/cpp/imgui_stdlib.h"

#include "modules/entity.h"
#include "modules/event.h"
#include "modules/resource.h"

namespace ikhanchoi {


std::unique_ptr<ManagerBase> WindowModule::generateManager() {
	return std::make_unique<WindowManager>();
}

/*----------*/
/* Resource */
/*----------*/

void WindowRenderer::visit(ModelResource& modelResource) const {
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

void WindowRenderer::visit(ShaderResource& shaderResource) const {
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
		ImGui::Text("shader OpenGL id: %d", shaderResource.getShaderObject());
	}
	ImGui::PopID();
}



/*-----------*/
/* Component */
/*-----------*/



/*--------*/
/* Entity */
/*--------*/

void WindowRenderer::visit(Entity& entity) const {
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
		auto* componentManager = getContext()->getManager<ComponentManager>();
		for (const auto& [componentType, componentHandle] : entity.getComponents())
			dynamic_cast<Object*>(componentManager->access<ComponentBase>(componentHandle))->accept(*this);
		ImGui::Text("Drag and drop to set components.");
		if (ImGui::BeginDragDropTarget()) { // TODO
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




/*--------*/
/* Window */
/*--------*/


void WindowRenderer::visit(ManagerWindow& managerWindow) const {
	ImGui::PushID(&managerWindow);
	ImGui::Begin(managerWindow.getName().c_str());
	auto* manager = managerWindow.getContent();
	// render the content depending on its manager type
	if (typeid(*manager) == typeid(ResourceManager)) {
		ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
		for (const auto &[objectType, pool]: manager->getPools()) {
			ImGui::PushID(objectType.name());
			if (ImGui::BeginTabItem(objectType.name())) {
				manager->forEach<ResourceBase>(objectType, [this](ResourceBase* upcastedResource) {
					auto* untypedResource = dynamic_cast<Object*>(upcastedResource);
					if (untypedResource->isActive())
						untypedResource->accept(*this);
				});
				ImGui::EndTabItem();
			}
			ImGui::PopID();
		}
		ImGui::EndTabBar();
	}
	else if (typeid(*manager) == typeid(EntityManager)) {
		auto entityManager = dynamic_cast<EntityManager*>(manager);
		for (const auto& entity : entityManager->getEntities()) {
			if (entity == nullptr)
				continue;
			entity->accept(*this);
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
				entityManager->addEntity(name, nullptr);
			}
			ImGui::SameLine();
			if (ImGui::Button("Cancel", ImVec2(-1, 0))
					|| (ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}

	}


	ImGui::End();
	ImGui::PopID();
}



/*----------------*/
/* Window Manager */
/*----------------*/


WindowManager::WindowManager() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	windowRenderer.setContext(context);
}

WindowManager::~WindowManager() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void WindowManager::render() {
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();
	ImGui::Begin("Window Manager");
	ImGui::Checkbox("Demo window", &demo);
	forEach<ManagerWindow>([](ManagerWindow* managerWindow) {
		ImGui::Checkbox((managerWindow->getName() + " window").c_str(), managerWindow->accessActive());
	});
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	if (demo)
		ImGui::ShowDemoWindow();
	forEach<ManagerWindow>([this](ManagerWindow* managerWindow) {
		if (managerWindow->isActive())
			managerWindow->accept(windowRenderer);
	});
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}




}