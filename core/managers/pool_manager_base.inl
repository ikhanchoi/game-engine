#include "pool_manager_base.h"

template <typename ObjectType>
void PoolManagerBase::registerPool() {
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
BaseType* PoolManagerBase::access(const Handle& handle) {
	if (pool.find(handle.type) == pool.end())
        throw std::runtime_error("Error: (Manager::access) Pool not found.");
	auto* untyped = pool[handle.type]->access(handle);
	auto* upcasted = dynamic_cast<BaseType*>(untyped);
	if (!upcasted)
		throw std::runtime_error("Error: (Manager::access) Failed to cast object to the specified type.");
	return upcasted;
}

template <typename BaseType, typename Function>
void PoolManagerBase::forEach(const std::type_index& objectType, Function&& function) {
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
void PoolManagerBase::forEach(Function&& function) {
	static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	if (pool.find(typeid(ObjectType)) == pool.end())
		throw std::runtime_error("Error: (Manager::forEach) Type not found in pool.");
	auto upcastedPool = pool.find(typeid(ObjectType))->second;
	auto typedPool = std::dynamic_pointer_cast<Pool<ObjectType>>(upcastedPool);
	typedPool->forEach([&](ObjectType* typedObject) {
		function(typedObject);
	});
}
