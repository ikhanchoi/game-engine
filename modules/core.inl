#pragma once

template <typename Type>
Handle Pool<Type>::add(Object* untypedObject) {
	static_assert(std::is_base_of_v<Object, Type>);
	static_assert(!std::is_abstract_v<Type>);
    Type* object = static_cast<Type*>(untypedObject);
    if (frees.empty()) {
        slots.push_back(Slot{ *object, 0, true });
        return { static_cast<uint32_t>(slots.size() - 1), 0, typeid(Type) };
    } else {
        uint32_t index = frees.back();
        frees.pop_back();
        slots[index].object = *object;
        slots[index].alive = true;
        return { index, slots[index].generation, typeid(Type) };
    }
}

template <typename Type>
void Pool<Type>::remove(const Handle& handle) {
	static_assert(std::is_base_of_v<Object, Type>);
	static_assert(!std::is_abstract_v<Type>);
    if (handle.index >= slots.size())
        throw std::runtime_error("Error: (Pool::remove) Invalid handle index.");
    Slot& slot = slots[handle.index];
    if (handle.generation != slot.generation || !slot.alive)
        throw std::runtime_error("Error: (Pool::remove) Invalid handle.");
    slot.alive = false;
    slot.generation++;
    frees.push_back(handle.index);
}

template <typename Type>
Object* Pool<Type>::access(const Handle& handle) {
	static_assert(std::is_base_of_v<Object, Type>);
	static_assert(!std::is_abstract_v<Type>);
    if (handle.index >= slots.size() || !slots[handle.index].alive)
        throw std::runtime_error("Error: (Pool::access) Invalid handle.");
    return &slots[handle.index].object;
}

template <typename Type>
void Pool<Type>::forEach(const std::function<void(Object*)>& function) {
	static_assert(std::is_base_of_v<Object, Type>);
	static_assert(!std::is_abstract_v<Type>);
	for (auto& slot : slots)
		if (slot.alive)
			function(&slot.object);
}


/*---------*/
/* Manager */
/*---------*/

template <typename Type>
void Manager::registerType(const std::string& typeName) {
	static_assert(std::is_base_of_v<Object, Type>);
	static_assert(!std::is_abstract_v<Type>);
    if (types.find(typeid(Type)) != types.end())
        throw std::runtime_error("Error: (Manager::registerType) Type already registered.");
    types.emplace(typeid(Type), typeName);
    pool[typeid(Type)] = std::make_unique<Pool<Type>>();
    factory[typeid(Type)] = [](uint32_t id, const std::string& name) {
        return std::make_unique<Type>(id, name);
    };
}

template <typename Module, typename Function>
void Manager::forEach(const std::type_index& type, Function&& function) {
	static_assert(std::is_base_of_v<Object, Module>);
	if (types.find(type) == types.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	pool.find(type)->second->forEach([&](Object* obj) {
        Module* basePtr = dynamic_cast<Module*>(obj);
        if (basePtr)
            function(basePtr);
    });

}