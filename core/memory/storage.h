#pragma once
#include "allocators/allocator.h"

template <typename ObjectType>
class Storage {
	std::unique_ptr<Allocator<ObjectType>> allocator;

	explicit Storage(std::unique_ptr<Allocator<ObjectType>> allocator) : allocator(std::move(allocator)) {}

public:
	template <template <typename> class AllocatorType>
	static Storage make(size_t initialCapacity = 64) {
		return Storage(std::make_unique<AllocatorType<ObjectType>>(initialCapacity));
	}

	Handle<ObjectType> create() { return allocator->create(); }
	Storage clone() { return Storage(allocator->clone()); }
	void destroy(Handle<ObjectType> handle) { allocator->destroy(handle); }
	void clear() { allocator->clear(); }

	bool valid(Handle<ObjectType> handle) { return allocator->valid(handle); }
	ObjectType* resolve(Handle<ObjectType> handle) { return allocator->resolve(handle); }
	std::vector<Handle<ObjectType>> view() { return allocator->view(); }
	void each(std::function<void(ObjectType*)> function) { allocator->each(function); }

/* TODO:
	void serialize(std::ostream& out) const { allocator->serialize(out); }
	void deserialize(std::istream& in) { allocator->deserialize(in);}
	size_t size() const { return allocator->size(); }
	size_t capacity() const { return allocator->capacity(); }
*/

};
