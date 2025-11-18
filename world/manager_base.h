#pragma once
#include "core/event/event_dispatcher.h"
#include "core/execution/command/command_buffer.h"
#include "core/memory/allocators/allocation_policies.h"
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

	template <typename EventType, typename... Args>
	void publish(Args&&... args) { eventDispatcher->publish<EventType>(std::forward<Args>(args)...); }
	template <typename Function>
	void submit(Function&& perform) { commandBuffer->submit(std::forward<Function>(perform)); }

	template <typename ObjectType>
	void registerStorage() {
		if (world.storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (ManagerBase::registerStorage) Storage already exists for: " + std::string(typeid(ObjectType).name()));
		auto storage = std::make_shared<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>();
		this->storage[typeid(ObjectType)] = std::weak_ptr<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>(storage);
		world.storage[typeid(ObjectType)] = std::move(storage);
	}

	template <typename ObjectType>
	Handle<ObjectType> create() { return _getStorage<ObjectType>()->create(); }
	template <typename ObjectType, template <typename> class AllocatorType>
	Storage<ObjectType, AllocatorType<ObjectType>> clone() { return _getStorage<ObjectType>()->clone(); }
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

	template <typename EventType>
	void subscribe(const std::function<void(const EventType&)>& handler) { eventDispatcher->subscribe<EventType>(handler); }
	void dispatch() const { eventDispatcher->dispatch(); }
	void flush() const { commandBuffer->flush(); }

	virtual ~ManagerBase() = 0;

private:
	template <typename ObjectType>
	auto _getStorage() {
		if (!storage.contains(typeid(ObjectType)))
			throw std::runtime_error("Error: (Manager::_getStorage) Storage does not exist for: " + std::string(typeid(ObjectType).name()));
		return std::any_cast<std::weak_ptr<Storage<ObjectType, typename AllocatorSelector<ObjectType>::type>>>(storage[typeid(ObjectType)]).lock();
	}
};

inline ManagerBase::~ManagerBase() = default;