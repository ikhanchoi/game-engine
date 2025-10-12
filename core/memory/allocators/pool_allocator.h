#pragma once
#include "allocator.h"

template <typename ObjectType>
class PoolAllocator final : public AllocatorCRTP<ObjectType, PoolAllocator> {
	friend class AllocatorCRTP<ObjectType, PoolAllocator>;
	std::vector<std::aligned_storage_t<sizeof(ObjectType), alignof(ObjectType)>> objects;
	std::vector<uint32_t> generations;
    std::vector<uint32_t> frees;
    std::vector<uint32_t> lives;
    std::vector<uint32_t> indices;

public:
	explicit PoolAllocator(size_t initialCapacity);
	~PoolAllocator() override;

protected:
	Handle<ObjectType> create() override;
	std::unique_ptr<Allocator<ObjectType>> clone() override;
	void destroy(Handle<ObjectType> handle) override;
	void clear() override;

	bool valid(Handle<ObjectType> handle) override;
	ObjectType* resolve(Handle<ObjectType> handle) override;
	std::vector<Handle<ObjectType>> view() override;
	template <typename Function>
	void _each(Function&& function);
};

#include "pool_allocator.inl"
