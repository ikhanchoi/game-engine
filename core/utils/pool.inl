#pragma once

template <typename ObjectType>
Pool<ObjectType>::Pool(size_t initialCapacity) {
	slots.reserve(initialCapacity);
	for (size_t i = 0; i < initialCapacity; ++i) {
		slots.emplace_back();
		frees.push_back(i);
	}
	std::reverse(frees.begin(), frees.end());
}

template <typename ObjectType>
Handle Pool<ObjectType>::add(Object* untypedObject) {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	ObjectType* typedObject = dynamic_cast<ObjectType*>(untypedObject);

	uint32_t index;
	if (!frees.empty()) {
		index = frees.back();
		frees.pop_back();
	} else {
		if (slots.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (Pool::add) Maximum pool size reached.");
		index = slots.size();
		slots.emplace_back();
	}
	Slot<ObjectType>& slot = slots[index];
	new (slot.objectBuffer) ObjectType(std::move(*typedObject)); // moved to avoid double deletion, placement new
	slot.alive = true;
	ObjectType* actualObjectInSlot = reinterpret_cast<ObjectType*>(slot.objectBuffer);
    actualObjectInSlot->setId(index);

	return { index, slot.generation, typeid(ObjectType) };
}

template <typename ObjectType>
void Pool<ObjectType>::remove(const Handle& handle) {
	if (handle.type != typeid(ObjectType))
        throw std::runtime_error("Error: (Pool::remove) Type mismatch for handle.");
    if (handle.index >= slots.size())
        throw std::runtime_error("Error: (Pool::remove) Invalid handle index.");
    Slot<ObjectType>& slot = slots[handle.index];
    if (handle.generation != slot.generation || !slot.alive)
        throw std::runtime_error("Error: (Pool::remove) Invalid handle.");
    slot.alive = false;
    slot.generation++;
	reinterpret_cast<ObjectType*>(slot.objectBuffer)->~ObjectType();
    frees.push_back(handle.index);
}

template <typename ObjectType>
Object* Pool<ObjectType>::access(const Handle& handle) {
	if (handle.type != typeid(ObjectType))
		throw std::runtime_error("Error: (Pool::access) Type mismatch for handle.");
    if (handle.index >= slots.size() || !slots[handle.index].alive)
        throw std::runtime_error("Error: (Pool::access) Invalid handle.");
	return reinterpret_cast<ObjectType*>(slots[handle.index].objectBuffer);
}

template <typename ObjectType>
void Pool<ObjectType>::forEach(const std::function<void(Object*)>& function) {
	for (auto& slot : slots)
		if (slot.alive)
			function(static_cast<Object*>(reinterpret_cast<ObjectType*>(slot.objectBuffer)));
}

template <typename ObjectType>
void Pool<ObjectType>::forEach(const std::function<void(ObjectType*)>& function) {
	for (auto& slot : slots)
		if (slot.alive)
			function(reinterpret_cast<ObjectType*>(slot.objectBuffer));
}