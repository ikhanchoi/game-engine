#pragma once
#include "core/memory/object.h"
#include "objects/entity/entity.h"




class WindowBase {
public:
	virtual ~WindowBase() = default;
};


class MainViewWindow : public WindowBase, public CRTPObject<MainViewWindow> {
public:
	explicit MainViewWindow(uint32_t id, const std::string& name) : CRTPObject<MainViewWindow>(id, name) {}


};



class AssetWindow : public WindowBase, public CRTPObject<AssetWindow> {
public:
	explicit AssetWindow(uint32_t id, const std::string& name) : CRTPObject<AssetWindow>(id, name) {}

};

class HierarchyWindow : public WindowBase, public CRTPObject<HierarchyWindow> {
public:
	explicit HierarchyWindow(uint32_t id, const std::string& name) : CRTPObject<HierarchyWindow>(id, name) {}

};


class InspectorWindow : public WindowBase, public CRTPObject<InspectorWindow> {
public:
	explicit InspectorWindow(uint32_t id, const std::string& name) : CRTPObject<InspectorWindow>(id, name) {}
};

class StatisticsWindow : public WindowBase, public CRTPObject<StatisticsWindow> {
public:
	explicit StatisticsWindow(uint32_t id, const std::string& name) : CRTPObject<StatisticsWindow>(id, name) {}
};







class WindowViewer : public AdapterVisitorBase {
	bool dockLayoutInitialized = false;
	bool inInspectorWindow = false;
	Object* selected = nullptr;
public:
	explicit WindowViewer(Context* context) : AdapterVisitorBase(context) {}
	void view();

	void visit(MainViewWindow* mainViewWindow) override;
	void visit(AssetWindow* assetWindow) override;
	void visit(HierarchyWindow* hierarchyWindow) override;
	void visit(InspectorWindow* inspectorWindow) override;
	void visit(StatisticsWindow* statisticsWindow) override;

	void visit(Entity* entity) override;
	void visit(ModelResource* modelResource) override;
	void visit(ShaderResource* shaderResource) override;
};


class WindowManager : public ManagerBase {
private:
	std::unordered_map<std::type_index, std::unique_ptr<WindowBase>> window;
	std::unique_ptr<WindowViewer> windowViewer;
public:
	explicit WindowManager(Context* context);
	~WindowManager() override;

	template <typename WindowType>
	WindowType* addWindow(std::string name) {
		static_assert(std::is_base_of<WindowBase, WindowType>::value, "WindowType must inherit from WindowBase.");
		if (window.find(typeid(WindowType)) != window.end())
			throw std::runtime_error("Error: (WindowManager::addWindow) Window already exists.");
		window[typeid(WindowType)] = std::make_unique<WindowType>(window.size(), name);
		return static_cast<WindowType*>(window[typeid(WindowType)].get());
	}

	std::unordered_map<std::type_index, std::unique_ptr<WindowBase>>& getWindows() { return window; }

	void view() { windowViewer->view(); }
	void subscribe() override {};

};



