#pragma once
#include "core/event/event_dispatcher.h"
#include "core/execution/command/command_buffer.h"
#include "core/memory/allocators/allocation_policies.h"
#include "core/memory/storage.h"
#include "world.h"

#include <any>
#include <typeindex>

class SystemBase {
	World& world;
	std::unique_ptr<EventDispatcher> eventDispatcher;
	std::unique_ptr<CommandBuffer> commandBuffer; // TODO: should be changed to an abstract class that supports async or job graph
	std::unordered_map<std::type_index, std::any> storage;

protected:
	explicit SystemBase(World& world) : world(world),
		eventDispatcher(std::make_unique<EventDispatcher>()), commandBuffer(std::make_unique<CommandBuffer>()) {}

	template <typename EventType, typename ... Args>
	void publish(Args&&... args) { eventDispatcher->publish<EventType>(std::forward<Args>(args)...); }
	template <typename Function>
	void submit(Function&& perform) { commandBuffer->submit(std::forward<Function>(perform)); }

	template <typename ObjectType>
	void registerStorage() {
		if (world.storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (SystemBase::registerStorage) Storage already exists for: " + std::string(typeid(ObjectType).name()));
		auto storage = std::make_shared<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>();
		this->storage[typeid(ObjectType)] = std::weak_ptr<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>(storage);
		world.storage[typeid(ObjectType)] = std::move(storage);
	}

public:
	template <typename ObjectType, typename Function>
	void each(Function&& function) { _getStorage<ObjectType>()->each(std::forward<Function>(function)); }

	virtual void tick() {}
	template <typename EventType>
	void subscribe(const std::function<void(const EventType&)>& handler) { eventDispatcher->subscribe<EventType>(handler); }
	void dispatch() const { eventDispatcher->dispatch(); }
	void flush() const { commandBuffer->flush(); }

	virtual ~SystemBase() = 0;

private:
	template <typename ObjectType>
	auto _getStorage() {
		if (!storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (Manager::_getStorage) Storage does not exist for: " + std::string(typeid(ObjectType).name()));
		return std::any_cast<std::weak_ptr<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>>(storage[typeid(ObjectType)]).lock();
	}
};

inline SystemBase::~SystemBase() = default;