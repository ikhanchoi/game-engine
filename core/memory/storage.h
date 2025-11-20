#pragma once
#include "allocators/allocator_concept.h"

template <typename ObjectType, typename ConcreteAllocatorType>
requires AllocatorConcept<ConcreteAllocatorType, ObjectType>
class Storage {
	ConcreteAllocatorType allocator;

public:
	explicit Storage(size_t initialCapacity = 64) : allocator(initialCapacity) {}

	Handle<ObjectType> create() { return allocator.create(); }
	std::unique_ptr<Storage> clone() { return std::make_unique<Storage>(allocator.clone()); }
	void destroy(Handle<ObjectType> handle) { allocator.destroy(handle); }
	void clear() { allocator->clear(); }

	bool valid(Handle<ObjectType> handle) { return allocator.valid(handle); }
	ObjectType* resolve(Handle<ObjectType> handle) { return allocator.resolve(handle); }
	std::vector<Handle<ObjectType>> view() { return allocator.view(); }
	template <typename Function>
	requires std::is_base_of_v<ComponentBase, ObjectType>
	void each(Function&& function) { allocator.each(std::forward<Function>(function)); }

/* TODO: allocator_concept
	void serialize(std::ostream& out) const { allocator->serialize(out); }
	void deserialize(std::istream& in) { allocator->deserialize(in);}
	size_t size() const { return allocator->size(); }
	size_t capacity() const { return allocator->capacity(); }
*/

};
