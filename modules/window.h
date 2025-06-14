#ifndef GAME_ENGINE_WINDOW_H
#define GAME_ENGINE_WINDOW_H

#include <utility>
#include <variant>

#include "modules/core.h"

namespace ikhanchoi {


class Window : public Object {
	std::variant<std::shared_ptr<Object>, std::shared_ptr<Manager>> content;
public:
	explicit Window(unsigned int id, const std::string& name) : Object(id, name) {}
	~Window() override = default;
	void update(Updater& updater) override { updater.update(*this); }
	void loadContent(std::shared_ptr<Manager> manager) { this->content = manager; }
	std::variant<std::shared_ptr<Object>, std::shared_ptr<Manager>> getContent() { return content; }
};

class WindowUpdater : public EmptyUpdater {
public:
	void update(ModelResource& modelResource) override;
	void update(ShaderResource& shaderResource) override;
	void update(ResourceManager& resourceManager) override;
	std::vector<std::string> resourceEventQueue;

	void update(Entity& entity) override;
	void update(EntityManager& entityManager) override;
	std::vector<std::string> entityEventQueue;

	void update(Window& window) override;
	void update(WindowManager& windowManager) override;
};




class WindowManager : public Manager {
private:
	bool demo = false;
	WindowUpdater windowUpdater;
public:
	explicit WindowManager(unsigned int id, const std::string& name);
	~WindowManager() override;
	void update(Updater& updater) override { updater.update(*this); }
	void update() { update(windowUpdater); };

	bool* accessDemo() { return &demo; }
	std::shared_ptr<Window> addWindow(const std::string& name, std::shared_ptr<Manager> manager) {
		if (types.find(typeid(Window)) == types.end())
			throw std::runtime_error("Error: (WindowManager::addWindow) Window type not registered.");
		auto window = dynamic_cast<Window*>(access(create(typeid(Window), name)));
		window->loadContent(std::move(manager));
		std::shared_ptr<void> dummy(nullptr, [](void*) {});
		std::shared_ptr<Window> alias(dummy, window);
		return alias;
	}
};



}

#endif//GAME_ENGINE_WINDOW_H
