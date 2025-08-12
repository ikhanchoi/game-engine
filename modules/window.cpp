#include "window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "core/context/context.h"
#include "modules/entity.h"
#include "modules/event.h"
#include "modules/resource.h"



#include <GLFW/glfw3.h>

namespace ikhanchoi {



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
				auto data = (std::pair<std::type_index, std::shared_ptr<ComponentBase>>*) payload->Data;
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




















/*---------*/
/* Windows */
/*---------*/


void WindowRenderer::visit(MainViewWindow& mainViewWindow) const {
	ImGui::PushID(&mainViewWindow);
	ImGui::Begin("Main View Window");
	ImGui::End();
	ImGui::PopID();
}


void WindowRenderer::visit(AssetWindow& assetWindow) const {

	ImGui::PushID(&assetWindow);
	ImGui::Begin("Asset Window");
	auto* resourceManager = assetWindow.getResourceManager();
	ImGui::BeginTabBar("Resources", ImGuiTabBarFlags_::ImGuiTabBarFlags_Reorderable);
	for (const auto &[objectType, pool]: resourceManager->getPools()) {
		ImGui::PushID(objectType.name());
		if (ImGui::BeginTabItem(objectType.name())) {
			resourceManager->forEach<ResourceBase>(objectType, [this](ResourceBase* upcastedResource) {
				auto* untypedResource = dynamic_cast<Object*>(upcastedResource);
				if (untypedResource->isActive())
					untypedResource->accept(*this);
			});
			ImGui::EndTabItem();
		}
		ImGui::PopID();
	}
	ImGui::EndTabBar();
	ImGui::End();
	ImGui::PopID();
}




void WindowRenderer::visit(HierarchyWindow& hierarchyWindow) const {
	ImGui::PushID(&hierarchyWindow);
	ImGui::Begin("Hierarchy Window");
	auto* entityManager = hierarchyWindow.getEntityManager();
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

	ImGui::End();
	ImGui::PopID();
}

void WindowRenderer::visit(InspectorWindow& inspectorWindow) const {
	ImGui::PushID(&inspectorWindow);
	ImGui::Begin("Inspector Window");

	ImGui::End();
	ImGui::PopID();
}



void WindowRenderer::visit(StatisticsWindow& statisticsWindow) const {
	ImGui::PushID(&statisticsWindow);
	ImGui::Begin("Statistics Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
	ImGui::PopID();
}



/*----------------*/
/* Window Manager */
/*----------------*/


WindowManager::WindowManager(Context* context) : ManagerBase(context), windowRenderer() {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
	windowRenderer.setContext(context);

	mainViewWindow = std::make_unique<MainViewWindow>(0, "Main View Window");
	assetWindow = std::make_unique<AssetWindow>(1, "Asset Window", context->getManager<ResourceManager>());
	hierarchyWindow = std::make_unique<HierarchyWindow>(2, "Hierarchy Window", context->getManager<EntityManager>());
	inspectorWindow = std::make_unique<InspectorWindow>(3, "Inspector Window", context->getManager<ComponentManager>());
	statisticsWindow = std::make_unique<StatisticsWindow>(4, "Statistics Window");


}

WindowManager::~WindowManager() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void WindowManager::render() {
	ImGui_ImplOpenGL3_NewFrame(), ImGui_ImplGlfw_NewFrame(), ImGui::NewFrame();

	ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                                    ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
                                    ImGuiWindowFlags_NoBackground;
    ImGui::Begin("DockSpace Window", nullptr, window_flags);
    ImGui::DockSpace(ImGui::GetID("DockSpace"), ImVec2(0.0f, 0.0f));
	if (!dockLayoutInitialized) {
		ImGuiID dock_id = ImGui::GetID("DockSpace");
		ImGui::DockBuilderRemoveNode(dock_id);
		ImGui::DockBuilderAddNode(dock_id, ImGuiDockNodeFlags_DockSpace);
		ImGui::DockBuilderSetNodeSize(dock_id, viewport->WorkSize);
		ImGuiID dock_id_inspector = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.20f, nullptr, &dock_id);
        ImGuiID dock_id_asset = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.25f, nullptr, &dock_id);
        ImGuiID dock_id_hierarchy = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.25f, nullptr, &dock_id);
		ImGui::DockBuilderDockWindow("Hierarchy Window", dock_id_hierarchy);
		ImGui::DockBuilderDockWindow("Asset Window", dock_id_asset);
		ImGui::DockBuilderDockWindow("Main View Window", dock_id);
		ImGui::DockBuilderDockWindow("Inspector Window", dock_id_inspector);
		ImGui::DockBuilderFinish(dock_id);
		dockLayoutInitialized = true;
	}
    ImGui::End();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {
			}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {
			}
			if (ImGui::MenuItem("Exit", "Alt+F4")) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {
			}
			if (ImGui::MenuItem("Redo", "Ctrl+Y", false, false)) {
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Windows")) {
			ImGui::MenuItem("Demo window", nullptr, &demoActive);
			ImGui::MenuItem("Asset window", nullptr, assetWindow->accessActive());
			ImGui::MenuItem("Hierarchy window", nullptr, hierarchyWindow->accessActive());
			ImGui::MenuItem("Inspector window", nullptr, inspectorWindow->accessActive());
			ImGui::MenuItem("Statistics window", nullptr, statisticsWindow->accessActive());
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (demoActive)
		ImGui::ShowDemoWindow();
	mainViewWindow->accept(windowRenderer);
	if (assetWindow->isActive())
		assetWindow->accept(windowRenderer);
	if (hierarchyWindow->isActive())
		hierarchyWindow->accept(windowRenderer);
	if (inspectorWindow->isActive())
		inspectorWindow->accept(windowRenderer);
	if (statisticsWindow->isActive())
		statisticsWindow->accept(windowRenderer);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}




}