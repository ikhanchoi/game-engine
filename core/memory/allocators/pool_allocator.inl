#pragma once

template <typename ObjectType>
PoolAllocator<ObjectType>::PoolAllocator(size_t initialCapacity) {
	objects.reserve(initialCapacity);
	generations.reserve(initialCapacity);
	lives.reserve(initialCapacity);
}

template <typename ObjectType>
PoolAllocator<ObjectType>::~PoolAllocator() {
	clear();
}

template <typename ObjectType>
Handle<ObjectType> PoolAllocator<ObjectType>::create() {
	uint32_t id;
	if (!frees.empty()) {
		id = frees.back();
		frees.pop_back();
	} else {
		if (objects.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (Pool<" + std::string(typeid(ObjectType).name()) +">::allocate) Pool size limit reached.");
		id = static_cast<uint32_t>(objects.size());
        objects.emplace_back();
		generations.push_back(0);
	}
	if (id >= indices.size())
		indices.resize(id + 1, UINT32_MAX);
	indices[id] = static_cast<uint32_t>(lives.size());
	lives.push_back(id);
	void* slot = &objects[id];
	new (slot) ObjectType(id);
	return Handle<ObjectType>(id, generations[id]);
}

template <typename ObjectType>
std::unique_ptr<Allocator<ObjectType>> PoolAllocator<ObjectType>::clone() {
	if constexpr (!std::is_copy_constructible_v<ObjectType>)
		throw std::runtime_error("Error: (Pool<" + std::string(typeid(ObjectType).name()) + ">::clone) ObjectType is not copy constructible.");
	auto newPool = std::make_unique<PoolAllocator>(objects.capacity());
	this->_each([&](ObjectType* object){
		ObjectType* newObject = newPool->resolve(newPool->create());
		new (newObject) ObjectType(*object);
	});
	return newPool;
}

template <typename ObjectType>
void PoolAllocator<ObjectType>::destroy(Handle<ObjectType> handle) {
	if (handle.id >= objects.size() || generations[handle.id] != handle.generation)
		throw std::runtime_error("Invalid or stale handle.");
	uint32_t id = handle.id;
	reinterpret_cast<ObjectType*>(&objects[id])->~ObjectType();
	++generations[id];
	frees.push_back(id);
	lives[indices[id]] = lives.back();
	indices[lives.back()] = indices[id];
	lives.pop_back();
	indices[id] = UINT32_MAX;
}

template <typename ObjectType>
void PoolAllocator<ObjectType>::clear() {
	for (auto& object : objects)
		reinterpret_cast<ObjectType*>(&object)->~ObjectType();
	objects.clear();
	generations.clear();
	frees.clear();
	lives.clear();
	indices.clear();
}

template<typename ObjectType>
bool PoolAllocator<ObjectType>::valid(Handle<ObjectType> handle) {
	return handle.id < objects.size() && generations[handle.id] == handle.generation; // really?
}

template <typename ObjectType>
ObjectType* PoolAllocator<ObjectType>::resolve(Handle<ObjectType> handle) {
	if (handle.id >= objects.size() || generations[handle.id] != handle.generation)
		throw std::runtime_error("Invalid or stale handle.");
    return reinterpret_cast<ObjectType*>(&objects[handle.id]);
}

template<typename ObjectType>
std::vector<Handle<ObjectType>> PoolAllocator<ObjectType>::view() {
	std::vector<Handle<ObjectType>> handles;
	for (auto id : lives)
		handles.emplace_back(id, generations[id]);
	return handles;
}

template <typename ObjectType>
template <typename Function>
void PoolAllocator<ObjectType>::_each(Function&& function) {
	for (auto id : lives)
		std::forward<Function>(function)(reinterpret_cast<ObjectType*>(&objects[id]));
}
