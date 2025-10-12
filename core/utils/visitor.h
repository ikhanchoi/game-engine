#ifndef GAME_ENGINE_VISITOR_H
#define GAME_ENGINE_VISITOR_H



class VisitorBase {
	class Context* context;
public:
	explicit VisitorBase(class Context* context) : context(context) {}
	virtual ~VisitorBase() = default;
	Context* getContext() { return context; }

	virtual void visit(class MainViewPanel* mainViewPanel)  = 0;
	virtual void visit(class AssetPanel* assetPanel)  = 0;
	virtual void visit(class HierarchyPanel* hierarchyPanel)  = 0;
	virtual void visit(class InspectorPanel* inspectorPanel)  = 0;
	virtual void visit(class StatisticsWindow* statisticsWindow)  = 0;

	virtual void visit(class ModelResource* modelResource)  = 0;
	virtual void visit(class ShaderResource* shaderResource)  = 0;

	virtual void visit(class Entity* entity)  = 0;

	virtual void visit(class RenderComponent* renderComponent)  = 0;
	virtual void visit(class TransformComponent* transformComponent) = 0;
	virtual void visit(class CameraComponent* cameraComponent)  = 0;
	virtual void visit(class LightComponent* lightComponent)  = 0;

};

class AdapterVisitorBase : public VisitorBase {
public:
	explicit AdapterVisitorBase(class Context* context) : VisitorBase(context) {}

	void visit(MainViewPanel* mainViewWindow) override {}
	void visit(AssetPanel* assetWindow) override {}
	void visit(HierarchyPanel* hierarchyWindow) override {}
	void visit(InspectorPanel* inspectorWindow) override {}
	void visit(StatisticsWindow* statisticsWindow) override {}

	void visit(ModelResource* modelResource) override {}
	void visit(ShaderResource* shaderResource) override {}

	void visit(Entity* entity) override {}
	void visit(RenderComponent* renderComponent) override {}
	void visit(TransformComponent* transformComponent) override {}
	void visit(CameraComponent* cameraComponent) override {}
	void visit(LightComponent* lightComponent) override {}
};






#endif//GAME_ENGINE_VISITOR_H
