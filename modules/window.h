#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <utility>
#include <variant>

#include "modules/core.h"

namespace ikhanchoi {

using Content = std::variant<Object*, ManagerBase*>;

class WindowModule : public CRTPModule<WindowModule> {
public:
	static std::unique_ptr<ManagerBase> generateManager();
};


class WindowBase : public Base {
public:
	virtual void loadContent(Content content) = 0;
};

class ManagerWindow : public WindowBase, public CRTPObject<ManagerWindow> {
	ManagerBase* content;
public:
	explicit ManagerWindow(uint32_t id, const std::string& name) : CRTPObject<ManagerWindow>(id, name) {}

	void loadContent(Content content) override {
		this->content = std::get<ManagerBase*>(content);
	}
	ManagerBase* getContent() { return content; }
	static std::string getTypeName() { return "ManagerWindow"; }
};



class WindowRenderer : public EmptyVisitor {
public:
	void visit(ManagerWindow& managerWindow) const override;

	void visit(ModelResource& modelResource) const override;
	void visit(ShaderResource& shaderResource) const override;

	void visit(Entity& entity) const override;

};



class WindowManager : public ManagerBase {
private:
	bool demo = false;
	WindowRenderer windowRenderer;
public:
	explicit WindowManager();
	~WindowManager() override;

	void render();

	Handle addWindow(const std::type_index& windowObjectType, const std::string& name, Content content) {
		if (pool.find(windowObjectType) == pool.end())
			throw std::runtime_error("Error: (WindowManager::addWindow) Window type not registered.");
		auto handle = create(windowObjectType, name);
		this->access<WindowBase>(handle)->loadContent(content);
		return handle;
	}
	template <typename WindowType>
	Handle addWindow(const std::string& name, Content content) {
		static_assert(std::is_base_of_v<WindowBase, WindowType>);
		return addWindow(typeid(WindowType), name, content);
	}
};



}

#endif//GAME_ENGINE_WINDOW_H
