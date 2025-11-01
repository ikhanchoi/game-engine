#pragma once
#include "world/system_base.h"

class HierarchyPanel : public SystemBase {
	class SceneManager& sceneManager;
	class EntityManager& entityManager;
	class InspectorPanel& inspectorPanel;

public:
	explicit HierarchyPanel(World& world);
	void tick() override;

private:
	void handleEntity(Tree<Handle<Entity>>::Node* node);
	void handlePopupContext(std::optional<Handle<Entity>> entity);
	void handleNewEntityPopup(std::optional<Handle<Entity>> entity);
	void handleDeleteEntityPopup(std::optional<Handle<Entity>> entity);
};