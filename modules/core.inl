#pragma once

template <typename Concrete>
Handle Pool<Concrete>::add(Object* object) {
    Concrete* typedObject = static_cast<Concrete*>(object);
    if (frees.empty()) {
        slots.push_back(Slot{ *typedObject, 0, true });
        return { static_cast<uint32_t>(slots.size() - 1), 0, typeid(Concrete) };
    } else {
        uint32_t index = frees.back();
        frees.pop_back();
        slots[index].object = *typedObject;
        slots[index].alive = true;
        return { index, slots[index].generation, typeid(Concrete) };
    }
}

template <typename Concrete>
void Pool<Concrete>::remove(const Handle& handle) {
    if (handle.index >= slots.size())
        throw std::runtime_error("Error: (Pool::remove) Invalid handle index.");
    Slot& slot = slots[handle.index];
    if (handle.generation != slot.generation || !slot.alive)
        throw std::runtime_error("Error: (Pool::remove) Invalid handle.");
    slot.alive = false;
    slot.generation++;
    frees.push_back(handle.index);
}

template <typename Concrete>
Object* Pool<Concrete>::access(const Handle& handle) {
    if (handle.index >= slots.size() || !slots[handle.index].alive)
        throw std::runtime_error("Error: (Pool::access) Invalid handle.");
    return &slots[handle.index].object;
}

template <typename Concrete>
void Pool<Concrete>::forEach(const std::function<void(Object*)>& function) {
	for (auto& slot : slots)
		if (slot.alive)
			function(&slot.object);
}

template <typename Concrete>
void Pool<Concrete>::forEach(const std::function<void(Concrete*)>& function) {
	for (auto& slot : slots)
		if (slot.alive)
			function(&slot.object);
}


/*---------*/
/* Manager */
/*---------*/



template <typename ParentType, typename ChildType>
void Manager::registerType() {
    static_assert(std::is_base_of_v<Object, ParentType>);
    static_assert(std::is_base_of_v<ParentType, ChildType>);
    auto parent = find<std::type_index>(root, typeid(ParentType));
    if (!parent)
        throw std::runtime_error("Error: Parent type not found in the type tree.");
    parent->children.push_back(std::make_shared<Tree<std::type_index>>(typeid(ChildType), ChildType::getTypeName()));
}


template <typename Concrete>
void Manager::registerPool() {
	static_assert(std::is_base_of_v<Object, Concrete>);
	static_assert(!std::is_abstract_v<Concrete>);
	if (!find<std::type_index>(root, typeid(Concrete)))
		throw std::runtime_error("Error: (Manager::registerPool) Type not found in the type tree." + std::string(typeid(Concrete).name()));
    if (pool.find(typeid(Concrete)) != pool.end())
        throw std::runtime_error("Error: (Manager::registerPool) Type already registered.");
    pool[typeid(Concrete)] = std::make_unique<Pool<Concrete>>();
    factory[typeid(Concrete)] = [](uint32_t id, const std::string& name) {
        return std::make_unique<Concrete>(id, name);
    };
}

template <typename Type>
Type* Manager::access(const Handle& handle) {
	static_assert(std::is_base_of_v<Object, Type>);
	if (pool.find(handle.type) == pool.end())
        throw std::runtime_error("Error: (Manager::access) Pool not found.");
	auto* typedObject = dynamic_cast<Type*>(pool[handle.type]->access(handle));
	if (!typedObject)
		throw std::runtime_error("Error: (Manager::access) Failed to cast object to the specified type.");
	return typedObject;
}

template <typename Type, typename Function>
void Manager::forEach(const std::type_index& concrete, Function&& function) {
	static_assert(std::is_base_of_v<Object, Type>);
	if (pool.find(concrete) == pool.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	pool.find(concrete)->second->forEach([&](Object* object) {
        Type* typedObject = dynamic_cast<Type*>(object);
        if (!typedObject)
			throw std::runtime_error("Error: (Manager::forEach) Failed to cast object to the specified type.");
        else
            function(typedObject);
    });
}

template <typename Concrete, typename Function>
void Manager::forEach(Function&& function) {
	static_assert(std::is_base_of_v<Object, Concrete>);
	static_assert(!std::is_abstract_v<Concrete>);
	if (pool.find(typeid(Concrete)) == pool.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	std::dynamic_pointer_cast<Pool<Concrete>>(pool[typeid(Concrete)])->forEach([&](Concrete* object) {
		function(object);
	});
}



/*---------*/
/* Context */
/*---------*/


template <typename Module>
void Context::registerModule() {
	if (manager.find(typeid(Module)) != manager.end())
		throw std::runtime_error("Error: (Context::registerModule) Manager type already registered.");
	manager[typeid(Module)] = Module::generateManager(); // if Module has no getModuleManager, it will throw an error
}


template <typename Module>
std::weak_ptr<Manager> Context::getManager() {
	if (manager.find(typeid(Module)) == manager.end())
		throw std::runtime_error("Error: (Context::getManager) Module not registered." + std::string(typeid(Module).name()));
	return this->manager[typeid(Module)];
}

template <typename ManagerType>
ManagerType* Context::access() {
	for (const auto& manager : this->manager)
		if (std::dynamic_pointer_cast<ManagerType>(manager.second))
			return dynamic_cast<ManagerType*>(&*manager.second);
	throw std::runtime_error("Error: (Context::access) Module not registered." + std::string(typeid(ManagerType).name()));
}