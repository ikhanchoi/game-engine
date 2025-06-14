#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <utility>
#include <variant>

#include "modules/core.h"

namespace ikhanchoi {

using Content = std::variant<std::weak_ptr<Object>, std::weak_ptr<Manager>>;

class Window : public Object {
	Content content;
public:
	explicit Window(unsigned int id, const std::string& name) : Object(id, name) {}
	~Window() override = default;
	void visit(Visitor& visitor) override { visitor.visit(*this); }
	static std::shared_ptr<Manager> generateManager() {
		return std::move(std::dynamic_pointer_cast<Manager>(std::make_shared<WindowManager>())); }
	static std::string getTypeName() { return "Window"; }

	void loadContent(Content content) { this->content = content; }
	Content getContent() { return content; }
};



class WindowRenderer : public EmptyVisitor {
public:
	void visit(ModelResource& modelResource) override;
	void visit(ShaderResource& shaderResource) override;
	void visit(ResourceManager& resourceManager) override;

	void visit(Entity& entity) override;
	void visit(EntityManager& entityManager) override;

	void visit(Window& window) override;
	void visit(WindowManager& windowManager) override;
};



class WindowManager : public Manager {
private:
	bool demo = false;
	WindowRenderer windowRenderer;
public:
	explicit WindowManager();
	~WindowManager() override;
	void visit(Visitor& visitor) override { visitor.visit(*this); }
	void render() { visit(windowRenderer); };

	bool* accessDemo() { return &demo; }

	Handle addWindow(const std::type_index& windowConcrete, const std::string& name, Content content) {
		if (pool.find(windowConcrete) == pool.end())
			throw std::runtime_error("Error: (WindowManager::addWindow) Window type not registered.");
		auto handle = create(windowConcrete, name);
		this->access<Window>(handle)->loadContent(content);
		return handle;
	}
	template <typename WindowType>
	Handle addWindow(const std::string& name, Content content) {
		static_assert(std::is_base_of_v<Window, WindowType>);
		return addWindow(typeid(WindowType), name, content);
	}
};



}

#endif//GAME_ENGINE_WINDOW_H
