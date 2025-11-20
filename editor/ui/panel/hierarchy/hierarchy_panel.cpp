#include "hierarchy_panel.h"

#include "core/containers/tree.h"

#include "world/scene/scene_manager.h"
#include "world/entity/entity_manager.h"
#include "editor/ui/panel/inspector/inspector_panel.h"

#include "include/imgui/imgui.h"



HierarchyPanel::HierarchyPanel(World& world) : SystemBase(world),
	sceneManager(world.get<SceneManager>()),
	entityManager(world.get<EntityManager>()),
	inspectorPanel(world.get<InspectorPanel>()){
}

void HierarchyPanel::tick() {
	ImGui::Begin("Hierarchy");
	if (!sceneManager.getCurrentScene().has_value()) {
		ImGui::End();
		return;
	}
	for (auto* tree : sceneManager.getEntityGraph()->trees)
		handleEntity(tree->root);

	ImGui::InvisibleButton("##invisible", ImGui::GetContentRegionAvail());
	if (ImGui::IsItemClicked())
		inspectorPanel.setSelected(std::nullopt);
	handlePopupContext(std::nullopt);

	ImGui::End();
}

void HierarchyPanel::handleEntity(Tree<Handle<Entity>>::Node* node) {
	ImGui::PushID(node);
	auto entity = node->value;
	std::string name = sceneManager.getEntityName(entity);
	if (name.empty())
		name = "Entity " + std::to_string(entity.id);

	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;
	if (inspectorPanel.getSelected() == entity)
		flags |= ImGuiTreeNodeFlags_Selected;
	bool open = ImGui::TreeNodeEx(name.c_str(), flags);
	if (ImGui::IsItemClicked())
		inspectorPanel.setSelected(entity);

	handlePopupContext(entity);

	if (open) {
		for (auto* child : node->children)
			handleEntity(child);
		ImGui::TreePop();
	}
	ImGui::PopID();
}

void HierarchyPanel::handlePopupContext(std::optional<Handle<Entity>> entity) {
	enum class Action {
		None,
		NewEntity,
		Delete
	} action = Action::None;

	if (ImGui::BeginPopupContextItem("##context")) {
		if (ImGui::MenuItem("New Entity"))
			action = Action::NewEntity;
		if (entity.has_value())
			if (ImGui::MenuItem("Delete"))
				action = Action::Delete;
		ImGui::EndPopup();
	}

	switch (action) {
		case Action::NewEntity:
			ImGui::OpenPopup("New entity");
			break;
		case Action::Delete:
			ImGui::OpenPopup("Delete entity");
			break;
		default:
			break;
	}

	handleNewEntityPopup(entity);
	handleDeleteEntityPopup(entity);
}

void HierarchyPanel::handleNewEntityPopup(std::optional<Handle<Entity>> entity) {
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (!ImGui::BeginPopupModal("New entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return;

	static char name[128] = "";
	if (ImGui::IsWindowAppearing())
		ImGui::SetKeyboardFocusHere();
	ImGui::InputText("Name", name, sizeof(name));

	if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x-ImGui::GetStyle().ItemSpacing.x)*0.5f,0))
		|| (ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused()))
	{
		ImGui::CloseCurrentPopup();
		entityManager.addEntity(std::string(name), entity);
		name[0] = '\0';
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(-1,0))
		|| (ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
	{
		ImGui::CloseCurrentPopup();
		name[0] = '\0';
	}

	ImGui::EndPopup();
}

void HierarchyPanel::handleDeleteEntityPopup(std::optional<Handle<Entity>> entity) {
	ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	if (!ImGui::BeginPopupModal("Delete entity", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		return;

	ImGui::Text("Are you sure you want to delete this entity?");

	if (ImGui::Button("OK", ImVec2((ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) * 0.5f, 0))
		|| (ImGui::IsKeyPressed(ImGuiKey_Enter) && ImGui::IsWindowFocused()))
	{
		ImGui::CloseCurrentPopup();
		if (entity.has_value()) {
			entityManager.removeEntity(entity.value());
			if (inspectorPanel.getSelected() == entity)
				inspectorPanel.setSelected(std::nullopt);
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Cancel", ImVec2(-1, 0))
		|| (ImGui::IsKeyPressed(ImGuiKey_Escape) && ImGui::IsWindowFocused()))
		ImGui::CloseCurrentPopup();

	ImGui::EndPopup();
}

// TODO:
// auto unfold parent nodes when a child is created?
// auto scroll to the newly constructed entity?
// auto focus to the newly constructed entity?
// inline rename mode by double click?
// drag and drop to reparent entities?
// multi-selection support?
