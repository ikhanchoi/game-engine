#ifndef GAME_ENGINE_EVENT_H
#define GAME_ENGINE_EVENT_H

#include <vector>
#include <unordered_map>
#include <typeindex>
#include <functional>
#include <queue>

#include "modules/window.h"


namespace ikhanchoi {

class Event { public: virtual ~Event() = default; };


class UIEvent : public Event {
public:
	// some info such as id
};

class EntityUIEvent : public UIEvent {
	// setName: entity, name
	// removeEntity: entity,
	// addEntity: entity,
	// setComponent: entity, component_type, component

};


class IEventSender {
public:
	virtual void publish(std::unique_ptr<Event> event, std::type_index eventType) = 0;
    virtual ~IEventSender() = default;
};

class EventSystem : public IEventSender {
private:
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<EntityManager> entityManager;

    std::unordered_map<std::type_index, std::vector<std::function<void(const Event&)>>> listeners;
	std::unordered_map<std::type_index, std::queue<std::unique_ptr<Event>>> eventQueue;
public:
	EventSystem() = default;
	~EventSystem() = default;

	void setResourceManager(std::shared_ptr<ResourceManager> resourceManager);
	void setEntityManager(std::shared_ptr<EntityManager> entityManager);



    template <typename EventType>
    using Listener = std::function<void(const EventType&)>;

    template <typename EventType>
    void subscribe(const Listener<EventType>& listener) {
        listeners[typeid(EventType)].push_back(
            [listener](const Event& event) {
                listener(static_cast<const EventType&>(event));
            }
        );
    }

	void publish(std::unique_ptr<Event> event, std::type_index eventType) override {
        eventQueue[eventType].push(std::move(event));
    }

    void process(std::type_index eventType) {
        while (!eventQueue[eventType].empty()) {
            std::unique_ptr<Event> event = std::move(eventQueue[eventType].front());
            eventQueue[eventType].pop();
			for (const auto& listener : listeners[eventType]) {
				listener(*event);
			}
			/*
			if (listeners.contains(typeid(*event)))
				for (const auto& listener: listeners[typeid(*event)])
					listener(*event);
			*/
		}
    }

};

}

#endif//GAME_ENGINE_EVENT_H
