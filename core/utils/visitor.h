#ifndef GAME_ENGINE_VISITOR_H
#define GAME_ENGINE_VISITOR_H

namespace ikhanchoi {

class Visitor {
public:
	virtual void visit(class MainViewWindow& mainViewWindow) const = 0;
	virtual void visit(class AssetWindow& assetWindow) const = 0;
	virtual void visit(class HierarchyWindow& hierarchyWindow) const = 0;
	virtual void visit(class InspectorWindow& inspectorWindow) const = 0;
	virtual void visit(class StatisticsWindow& statisticsWindow) const = 0;

	virtual void visit(class ModelResource& modelResource) const = 0;
	virtual void visit(class ShaderResource& shaderResource) const = 0;

	virtual void visit(class Entity& entity) const = 0;

	virtual void visit(class RenderComponent& renderComponent) const = 0;
	virtual void visit(class TransformComponent& transformComponent) const = 0;
	virtual void visit(class CameraComponent& cameraComponent) const = 0;
	virtual void visit(class LightComponent& lightComponent) const = 0;

};

class EmptyVisitor : public Visitor {
	class Context* context = nullptr;
public:
	void setContext(class Context* context) { this->context = context; }
	Context* getContext() const { return context; }

	void visit(MainViewWindow& mainViewWindow) const override {}
	void visit(AssetWindow& assetWindow) const override {}
	void visit(HierarchyWindow& hierarchyWindow) const override {}
	void visit(InspectorWindow& inspectorWindow) const override {}
	void visit(StatisticsWindow& statisticsWindow) const override {}

	void visit(ModelResource& modelResource) const override {}
	void visit(ShaderResource& shaderResource) const override {}

	void visit(Entity& entity) const override {}

	void visit(RenderComponent& renderComponent) const override {}
	void visit(TransformComponent& transformComponent) const override {}
	void visit(CameraComponent& cameraComponent) const override {}
	void visit(LightComponent& lightComponent) const override {}
};

class WindowRenderer : public EmptyVisitor {
public:
	void visit(MainViewWindow& mainViewWindow) const override;
	void visit(AssetWindow& assetWindow) const override;
	void visit(HierarchyWindow& hierarchyWindow) const override;
	void visit(InspectorWindow& inspectorWindow) const override;
	void visit(StatisticsWindow& statisticsWindow) const override;

	void visit(ModelResource& modelResource) const override;
	void visit(ShaderResource& shaderResource) const override;

	void visit(Entity& entity) const override;

};

}


#endif//GAME_ENGINE_VISITOR_H
