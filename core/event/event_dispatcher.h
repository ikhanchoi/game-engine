#pragma once
#include "event_base.h"

class EventDispatcher {
	std::vector<std::unique_ptr<EventBase>> events;
	std::unordered_map<std::type_index, std::vector<std::function<void(const EventBase&)>>> listeners;

public:
	template <typename EventType>
	void subscribe(const std::function<void(const EventType&)>& handler) {
		listeners[typeid(EventType)].push_back([handler](const EventBase& event) {
			handler(static_cast<const EventType&>(event));
		});
	}

	template <typename EventType, typename... Args>
	void publish(Args&&... args) {
		events.push_back(std::make_unique<EventType>(std::forward<Args>(args)...));
	}

	void dispatch() {
		for (auto& event : events) {
			if (!event)
				continue;
			auto& e = *event;
			if (auto it = listeners.find(typeid(e)); it != listeners.end())
				for (auto& handler : it->second)
					handler(*event);
		}
		events.clear(); // TODO: deferred event should be handled.
	}
};