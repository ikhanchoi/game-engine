#include "inspector_panel.h"
#include "core/execution/command/command.h"

#include "include/imgui/imgui.h"


void InspectorPanel::tick() {
	ImGui::Begin("Inspector");
	if (selected.has_value()) {
		ImGui::Text("Entity selected");
	}
	ImGui::End();
}



void InspectorPanel::setSelected(std::optional<Handle<Entity>> entity) {
	submit([this, entity] {
		selected = entity;
	});
}


std::optional<Handle<Entity>> InspectorPanel::getSelected() const {
	return selected;
}