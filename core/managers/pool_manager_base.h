#ifndef GAME_ENGINE_POOL_MANAGER_BASE_H
#define GAME_ENGINE_POOL_MANAGER_BASE_H

#include <unordered_map>
#include <typeindex>
#include <functional>

#include "core/utils/pool.h"
#include "manager_base.h"

namespace ikhanchoi {

class PoolManagerBase : public ManagerBase {

protected:
	std::unordered_map<std::type_index, std::shared_ptr<PoolBase>> pool;
    std::unordered_map<std::type_index, std::function<std::unique_ptr<Object>(uint32_t, const std::string&)>> factory;
protected:
	Handle create(const std::type_index& type, const std::string& name); // creator for objects
	void destroy(const Handle& handle); // destroyer for objects
public:
	explicit PoolManagerBase(Context* context) : ManagerBase(context), pool(), factory() {}

	// setters and getters
	template <typename ObjectType>
	void registerPool();
	const std::unordered_map<std::type_index, std::shared_ptr<PoolBase>>& getPools() { return pool; } // for iteration

	template <typename BaseType>
	BaseType* access(const Handle& handle); // accessor for objects without directly getting pool by handle

	// for-each loops
	template <typename BaseType, typename Function>
	void forEach(const std::type_index& objectType, Function&& function);
	template <typename ObjectType, typename Function>
	void forEach(Function&& function);
};

#include "pool_manager_base.inl"

}


#endif//GAME_ENGINE_POOL_MANAGER_BASE_H