#pragma once

template <typename ObjectType>
Handle Pool<ObjectType>::add(Object* untypedObject) {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	ObjectType* typedObject = dynamic_cast<ObjectType*>(untypedObject);

	uint32_t index;
	if (!frees.empty()) {
		index = frees.back();
		frees.pop_back();
	} else {
		std::cerr << "Warning: Pool for " << typeid(ObjectType).name() << " is expanding. Consider increasing initial capacity." << std::endl;
		index = slots.size();
		slots.emplace_back();
	}
	Slot& slot = slots[index];
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
    Slot& slot = slots[handle.index];
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




/*---------*/
/* Manager */
/*---------*/



template <typename ObjectType>
void ManagerBase::registerObjectType() {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
    if (pool.find(typeid(ObjectType)) != pool.end())
        throw std::runtime_error("Error: (Manager::registerType) Type already registered.");

	if constexpr (!std::is_same_v<ObjectType, Entity>)
        pool[typeid(ObjectType)] = std::make_unique<Pool<ObjectType>>();

    factory[typeid(ObjectType)] = [](uint32_t id, const std::string& name) {
        return std::make_unique<ObjectType>(id, name);
    };
}

template <typename BaseType> // e.g. `ResourceBase`
BaseType* ManagerBase::access(const Handle& handle) {
	static_assert(std::is_base_of_v<Base, BaseType>);
	if (pool.find(handle.type) == pool.end())
        throw std::runtime_error("Error: (Manager::access) Pool not found.");
	auto* untyped = pool[handle.type]->access(handle);
	auto* upcasted = dynamic_cast<BaseType*>(untyped);
	if (!upcasted)
		throw std::runtime_error("Error: (Manager::access) Failed to cast object to the specified type.");
	return upcasted;
}

template <typename BaseType, typename Function>
void ManagerBase::forEach(const std::type_index& objectType, Function&& function) {
	if (pool.find(objectType) == pool.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	auto upcastedPool = pool.find(objectType)->second;
	upcastedPool->forEach([&](Object* untypedObject) {
        auto* upcastedObject = dynamic_cast<BaseType*>(untypedObject);
        if (!upcastedObject)
			throw std::runtime_error("Error: (Manager::forEach) Failed to cast object to the specified type.");
        function(upcastedObject);
    });
}

template <typename ObjectType, typename Function>
void ManagerBase::forEach(Function&& function) {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	if (pool.find(typeid(ObjectType)) == pool.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	auto upcastedPool = pool.find(typeid(ObjectType))->second;
	auto typedPool = std::dynamic_pointer_cast<Pool<ObjectType>>(upcastedPool);
	typedPool->forEach([&](ObjectType* typedObject) {
		function(typedObject);
	});
}



/*---------*/
/* Context */
/*---------*/


template <typename ModuleType>
void Context::registerModuleType() {
	auto newManager = ModuleType::generateManager(this);
	auto& managerType = typeid(*newManager);
	if (manager.find(managerType) != manager.end())
		throw std::runtime_error("Error: (Context::registerModule) Manager type already registered.");
	manager[managerType] = std::move(newManager);
	if (!manager[managerType])
		throw std::runtime_error("Error: (Context::registerModule) Failed to create manager.");
}

template <typename ManagerType>
ManagerType* Context::getManager() {
	static_assert(std::is_base_of_v<ManagerBase, ManagerType>);
	return dynamic_cast<ManagerType*>(getManager(typeid(ManagerType)));
}