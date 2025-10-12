#include "window.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "imgui_internal.h"
#include "misc/cpp/imgui_stdlib.h"

#include "core/context/context.h"
#include "modules/event.h"
#include "objects/resource/resource.h"
#include "objects/entity/entity.h"


#include <GLFW/glfw3.h>





/*----------*/
/* Resource */
/*----------*/

void WindowViewer::visit(ModelResource* modelResource) {
	ImGui::PushID(modelResource);
	if (!inInspectorWindow) {
		if (modelResource->isActive()) {
			ImGui::BeginGroup();
			if (modelResource->getIconTexture())
				ImGui::ImageButton("", modelResource->getIconTexture(), ImVec2(64, 64));// ?
			else
				ImGui::Button("No Icon", ImVec2(64, 64));
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("ModelResource", modelResource, sizeof(*modelResource));
				ImGui::Text("Put it into render components: %s", modelResource->getName().c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::TextWrapped("%s", modelResource->getName().c_str());
			ImGui::EndGroup();
			ImGui::SameLine();
		}
	} else { // TODO: show more information for each buffer and texture objects.
		ImGui::Text("number of buffer objects: %d", (int) modelResource->getBufferObjects().size());
		ImGui::Text("number of texture objects: %d", (int) modelResource->getTextureObjects().size());
	}
	ImGui::PopID();
}

void WindowViewer::visit(ShaderResource* shaderResource) {
	ImGui::PushID(shaderResource);
	if (!inInspectorWindow) {
		if (shaderResource->isActive()) {
			ImGui::BeginGroup();
			if (shaderResource->getIconTexture())
				ImGui::ImageButton("", shaderResource->getIconTexture(), ImVec2(64, 64));// ?
			else
				ImGui::Button("No Icon", ImVec2(64, 64));
			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
				ImGui::SetDragDropPayload("ShaderResource", shaderResource, sizeof(*shaderResource));
				ImGui::Text("Put it into render components: %s", shaderResource->getName().c_str());
				ImGui::EndDragDropSource();
			}
			ImGui::TextWrapped("%s", shaderResource->getName().c_str());
			ImGui::EndGroup();
			ImGui::SameLine();
		}
	} else {
		ImGui::Text("shader OpenGL id: %d", shaderResource->getShaderObject());
	}
	ImGui::PopID();
}



/*-----------*/
/* Component */
/*-----------*/


/*--------*/
/* Entity */
/*--------*/

void WindowViewer::visit(Entity* entity) {
	ImGui::PushID(entity);
	if (!inInspectorWindow) {
		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow
							| ImGuiTreeNodeFlags_SpanAvailWidth
                            | (selected == entity ? ImGuiTreeNodeFlags_Selected : 0);

		bool open = ImGui::TreeNodeEx((void*)entity, flags, "%s", entity->getName().c_str());
		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
			selected = entity;
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
			ImGui::SetDragDropPayload("Entity", reinterpret_cast<const void*>(&entity), sizeof(Entity*));
			ImGui::Text("%s", entity->getName().c_str());
			ImGui::EndDragDropSource();
		}
		if (ImGui::BeginDragDropTarget()) { // runtime error. we need to process this with events.
			if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Entity")) {
				auto* dropped = *(Entity**)payload->Data;
                bool dropAbove = ImGui::GetMousePos().y < (ImGui::GetItemRectMin().y + ImGui::GetItemRectMax().y) * 0.5f;
				auto* eventManager = dynamic_cast<EventManager*>(getContext()->getManager(typeid(EventManager)));
				if (dropped->getParent() == entity->getParent() && dropped != entity) {
                    auto event = std::make_unique<ReorderEntityEvent>(dropped, entity, dropAbove);
                    eventManager->post(std::move(event));
                } else {
                    auto event = std::make_unique<ReparentEntityEvent>(dropped, dropAbove ? entity->getParent() : entity);
                    eventManager->post(std::move(event));
                }
			}
			ImGui::EndDragDropTarget();
		}
		if (open) {
			for (auto& child : entity->getChildren())
				child->accept(*this);
			for (auto& [componentType, componentHandle] : entity->getComponents()) {
				auto* componentManager = dynamic_cast<ComponentManager *>(getContext()->getManager(typeid(ComponentManager)));
				dynamic_cast<Object *>(componentManager->access<ComponentBase>(componentHandle))->accept(*this);
			}
			ImGui::TreePop();
		}

	} else {
		ImGui::SetNextItemOpen(true, ImGuiCond_Once);
		bool open = ImGui::CollapsingHeader((entity->getName() + " (ID: " + std::to_string(entity->getId()) + ")").c_str());
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
			ImGui::Checkbox("Active", entity->accessActive());
			auto *componentManager = dynamic_cast<ComponentManager *>(getContext()->getManager(typeid(ComponentManager)));
			for (const auto &[componentType, componentHandle]: entity->getComponents())
				dynamic_cast<Object *>(componentManager->access<ComponentBase>(componentHandle))->accept(*this);
			ImGui::Text("Drag and drop to set components.");
			if (ImGui::BeginDragDropTarget()) {// TODO
				if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Component")) {
					auto data = (std::pair<std::type_index, std::shared_ptr<ComponentBase>> *) payload->Data;
					auto componentType = (*data).first;
					auto component = (*data).second;
					// set components event?
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
			if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, 0)) ||
				(ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused())) {
				ImGui::CloseCurrentPopup();
				entity->setName(name);// edit name event?
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
			if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, 0)) ||
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
	}
	ImGui::PopID();
}




















/*---------------*/
/* Window Viewer */
/*---------------*/


void WindowViewer::visit(MainViewWindow* mainViewWindow) {
	ImGui::PushID(mainViewWindow);
	ImGui::Begin("Main View Window");
	float windowHeight = ImGui::GetMainViewport()->Size.y;
	ImVec2 mainViewPos = ImGui::GetWindowPos();
	ImVec2 mainViewSize = ImGui::GetContentRegionAvail();
	// mainViewWindow.setViewPort((int)mainViewPos.x, (int)(windowHeight - mainViewPos.y - mainViewSize.y), (int)mainViewSize.x, (int)mainViewSize.y);
	ImGui::End();
	ImGui::PopID();
}


void WindowViewer::visit(AssetWindow* assetWindow) {
	ImGui::PushID(assetWindow);
	ImGui::Begin("Asset Window");
	auto* resourceManager = dynamic_cast<ResourceManager*>(getContext()->getManager(typeid(ResourceManager)));
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




void WindowViewer::visit(HierarchyWindow* hierarchyWindow) {
	ImGui::PushID(hierarchyWindow);
	ImGui::Begin("Hierarchy Window");
	auto* entityManager = dynamic_cast<EntityManager*>(getContext()->getManager(typeid(EntityManager)));
	for (const auto& entity : entityManager->getEntities())
		entity->accept(*this);

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


void WindowViewer::visit(InspectorWindow* inspectorWindow) {
	ImGui::PushID(inspectorWindow);
	ImGui::Begin("Inspector Window");
	inInspectorWindow = true;
	if (selected)
		selected->accept(*this);
	inInspectorWindow = false;
	ImGui::End();
	ImGui::PopID();
}



void WindowViewer::visit(StatisticsWindow* statisticsWindow) {
	ImGui::PushID(statisticsWindow);
	ImGui::Begin("Statistics Window");
	ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
	ImGui::End();
	ImGui::PopID();
}



void WindowViewer::view() {
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
		ImGuiID dock_id_inspector = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Right, 0.15f, nullptr, &dock_id);
        ImGuiID dock_id_asset = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Down, 0.20f, nullptr, &dock_id);
        ImGuiID dock_id_hierarchy = ImGui::DockBuilderSplitNode(dock_id, ImGuiDir_Left, 0.15f, nullptr, &dock_id);
		ImGui::DockBuilderDockWindow("Hierarchy Window", dock_id_hierarchy);
		ImGui::DockBuilderDockWindow("Asset Window", dock_id_asset);
		ImGui::DockBuilderDockWindow("Main View Window", dock_id);
		ImGui::DockBuilderDockWindow("Inspector Window", dock_id_inspector);
		ImGui::DockBuilderFinish(dock_id);
		dockLayoutInitialized = true;
	}
    ImGui::End();

	auto* windowManager = dynamic_cast<WindowManager*>(getContext()->getManager(typeid(WindowManager)));
	auto& window = windowManager->getWindows();
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
			for (const auto& [windowType, upcastedWindow] : window) {
				if (windowType == typeid(MainViewWindow))
					continue;
				auto* untypedWindow = dynamic_cast<Object*>(upcastedWindow.get());
				ImGui::MenuItem(untypedWindow->getName().c_str(), nullptr, untypedWindow->accessActive());
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	for (const auto& [windowType, upcastedWindow] : window) {
		auto* untypedWindow = dynamic_cast<Object*>(upcastedWindow.get());
		if (untypedWindow->isActive() || windowType == typeid(MainViewWindow))
			untypedWindow->accept(*this);
	}

	ImGui::ShowDemoWindow();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


/*----------------*/
/* Window Manager */
/*----------------*/


WindowManager::WindowManager(Context* context) : ManagerBase(context), windowViewer(std::make_unique<WindowViewer>(context)) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(glfwGetCurrentContext(), true);
	ImGui_ImplOpenGL3_Init("#version 330");
}

WindowManager::~WindowManager() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}



