#pragma once
#include "../core/execution/events/event_dispatcher.h"
#include "core/execution/commands/command_buffer.h"
#include "core/memory/storage.h"
#include "world.h"

#include <any>
#include <typeindex>

class ManagerBase {
	World& world;
	std::unique_ptr<EventDispatcher> eventDispatcher;
	std::unique_ptr<CommandBuffer> commandBuffer;
	std::unordered_map<std::type_index, std::any> storage; // storage pointers that can be accessed by this manager instance.

protected:
	explicit ManagerBase(World& world) : world(world),
		eventDispatcher(std::make_unique<EventDispatcher>()), commandBuffer(std::make_unique<CommandBuffer>()) {}

	// access to world
	template <ManagerOrSystem Type>
	Type* get() { return world.get<Type>(); }

	// access to event dispatcher
	template <typename EventType, typename ... Args>
	void publish(Args&&... args) { eventDispatcher->publish<EventType>(std::forward<Args>(args)...); }

	// access to command buffer
	template <typename CommandType, typename... Args>
	void push(Args&&... args) { commandBuffer->push(std::make_unique<CommandType>(std::forward<Args>(args)...)); }

	// access to storage
	template <typename ObjectType, template <typename> class AllocatorType>
	void registerStorage() {
		if (world.storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (Manager::registerStorage) Storage already exists for: " + std::string(typeid(ObjectType).name()));
		auto storage = std::make_shared<Storage<ObjectType>>(Storage<ObjectType>::template make<AllocatorType>());
		this->storage[typeid(ObjectType)] = std::weak_ptr<Storage<ObjectType>>(storage);
		world.storage[typeid(ObjectType)] = std::move(storage);
	}

	template <typename ObjectType>
	Handle<ObjectType> create() { return _getStorage<ObjectType>()->create(); }
	template <typename ObjectType>
	Storage<ObjectType> clone() { return _getStorage<ObjectType>()->clone(); }
	template <typename ObjectType>
	void destroy(Handle<ObjectType> handle) { _getStorage<ObjectType>()->destroy(handle); }
	template <typename ObjectType>
	void clear() { _getStorage<ObjectType>()->clear(); }

public:
	template <typename ObjectType>
	bool valid(Handle<ObjectType> handle) { return _getStorage<ObjectType>()->valid(handle); }
	template <typename ObjectType>
	ObjectType* resolve(Handle<ObjectType> handle) { return _getStorage<ObjectType>()->resolve(handle); }
	template <typename ObjectType>
	std::vector<Handle<ObjectType>> view() { return _getStorage<ObjectType>()->view(); }
	template <typename ObjectType>
	void each(std::function<void(ObjectType*)> function) { _getStorage<ObjectType>()->each(function); }

	// access from world
	template <typename EventType>
	void subscribe(const std::function<void(const EventType&)>& handler) { eventDispatcher->subscribe<EventType>(handler); }
	void dispatch() const { eventDispatcher->dispatch(); }
	void flush() const { commandBuffer->flush(); }

	virtual ~ManagerBase() = 0;

private:
	template <typename ObjectType>
	std::shared_ptr<Storage<ObjectType>> _getStorage() {
		if (!storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (Manager::_getStorage) Storage does not exist for: " + std::string(typeid(ObjectType).name()));
		return std::any_cast<std::weak_ptr<Storage<ObjectType>>>(storage[typeid(ObjectType)]).lock();
	}
};

inline ManagerBase::~ManagerBase() = default;