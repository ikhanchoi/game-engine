template <typename ObjectType>
template <typename... Args>
Handle<ObjectType> FreeList<ObjectType>::allocate(Args&&... args) {
	uint32_t index;
	if (!frees.empty()) {
		index = frees.back();
		frees.pop_back();
	} else {
		if (objects.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (FreeList<" + std::string(typeid(ObjectType).name()) + ">::allocate) FreeList size limit reached.");
		index = static_cast<uint32_t>(objects.size());
		objects.emplace_back();
		generations.push_back(0);
	}
	objects[index].emplace(std::forward<Args>(args)...);
	return { index, generations[index] };
}

template <typename ObjectType>
void FreeList<ObjectType>::deallocate(const Handle<ObjectType>& handle) {
	if (!isValid(handle))
		throw std::runtime_error("Invalid or stale handle.");
	uint32_t index = handle.index;
	objects[index].reset();
	generations[index]++;
	frees.push_back(index);
}

template <typename ObjectType>
ObjectType* FreeList<ObjectType>::resolve(const Handle<ObjectType>& handle) {
	if (!isValid(handle))
		throw std::runtime_error("Invalid or stale handle.");
	return objects[handle.index] ? &(*objects[handle.index]) : nullptr;
}

template <typename ObjectType>
void FreeList<ObjectType>::forEach(const std::function<void(ObjectType*)>& function) {
	for (auto& object : objects) {
		if (object)
			function(&(*object));
	}
}

template <typename ObjectType>
bool FreeList<ObjectType>::isValid(const Handle<ObjectType>& handle) const {
	return  handle.index < objects.size() &&
			generations[handle.index] == handle.generation &&
			objects[handle.index].has_value();
}