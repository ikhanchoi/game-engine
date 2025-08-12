#include "pool_manager_base.h"

namespace ikhanchoi {

Handle PoolManagerBase::create(const std::type_index& objectType, const std::string& name) {
    if (pool.find(objectType) == pool.end())
		throw std::runtime_error("Error: (Manager::create) Type Pool not registered for: " + std::string(objectType.name()));
    std::unique_ptr<Object> object = factory[objectType](0, name);
    if (!object)
        throw std::runtime_error("Error: (Manager::create) Failed to create object.");
    Handle handle = pool[objectType]->add(object.get());
    return handle;
}

void PoolManagerBase::destroy(const Handle& handle) {
	if (pool.find(handle.type) == pool.end())
        throw std::runtime_error("Error: (Manager::destroy) Module not found.");
    pool[handle.type]->remove(handle);
}

}