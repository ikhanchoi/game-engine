#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <utility>
#include <variant>

#include "modules/core.h"

namespace ikhanchoi {

using Content = std::variant<Object*, ManagerBase*>;

class WindowModule : public CRTPModule<WindowModule> {
public:
	static std::unique_ptr<ManagerBase> generateManager(Context* context);
};


class WindowBase : public Base {
public:
};





class AssetWindow : public WindowBase, public CRTPObject<AssetWindow> {
	ResourceManager* resourceManager;
public:
	explicit AssetWindow(uint32_t id, const std::string& name, ResourceManager* resourceManager = nullptr)
		: CRTPObject<AssetWindow>(id, name), resourceManager(resourceManager) {}

	ResourceManager* getResourceManager() const { return resourceManager; }
};

class HierarchyWindow : public WindowBase, public CRTPObject<HierarchyWindow> {
	EntityManager* entityManager;
public:
	explicit HierarchyWindow(uint32_t id, const std::string& name, EntityManager* entityManager = nullptr)
		: CRTPObject<HierarchyWindow>(id, name), entityManager(entityManager) {}

	EntityManager* getEntityManager() const { return entityManager; }
};


class InspectorWindow : public WindowBase, public CRTPObject<InspectorWindow> {
	ComponentManager* componentManager;
public:
	explicit InspectorWindow(uint32_t id, const std::string& name, ComponentManager* componentManager = nullptr)
		: CRTPObject<InspectorWindow>(id, name), componentManager(componentManager) {}

	ComponentManager* getComponentManager() const { return componentManager; }
};

class StatisticsWindow : public WindowBase, public CRTPObject<StatisticsWindow> {
public:
	explicit StatisticsWindow(uint32_t id, const std::string& name)
		: CRTPObject<StatisticsWindow>(id, name) {}
};



class WindowManager : public ManagerBase {
private:
	std::unique_ptr<AssetWindow> assetWindow;
	std::unique_ptr<HierarchyWindow> hierarchyWindow;
	std::unique_ptr<InspectorWindow> inspectorWindow;
	std::unique_ptr<StatisticsWindow> statisticsWindow;
	bool demoActive = false;
	WindowRenderer windowRenderer;
public:
	explicit WindowManager(Context* context);
	~WindowManager() override;

	void render();

};



}

#endif//GAME_ENGINE_WINDOW_H
