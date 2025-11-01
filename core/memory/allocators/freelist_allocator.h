#pragma once
// 재사용이 불규칙적이고, 크기가 동적으로 변하며, 순회보다는 개별 접근이 많은 경우, 단발성 임시 리소스 등에 적합.

template <typename ObjectType>
class FreeListAllocator final {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	std::vector<std::optional<ObjectType>> objects;
	std::vector<uint32_t> generations;
	std::vector<uint32_t> frees;

public:
	explicit FreeList() = default;
	~FreeList() override = default;
	// TODO: remove template for handles
	void clear() override;
	Handle create() override;
	void destroy(const Handle& handle) override;
	ObjectType* resolve(const Handle& handle);
	void forEach(const std::function<void(ObjectType*)>& function);
	bool valid(const Handle& handle) const;
};

#include "../freelist_allocator.inl"
