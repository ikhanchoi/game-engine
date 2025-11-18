#pragma once
#include "core/containers/tree.h"
#include <map>
#include <any>

template<typename Type>
concept ManagerOrSystem = std::is_base_of_v<class ManagerBase, Type> || std::is_base_of_v<class SystemBase, Type>;

class World {
	std::unordered_map<std::type_index, std::any> storage; // TODO: resolve external fragmentation
	std::map<std::type_index, std::unique_ptr<ManagerBase>> manager;
	std::unordered_map<std::type_index, std::unique_ptr<SystemBase>> system;

	Forest<std::type_index> dependencyGraph; // for systems TODO: DAG

public:
	void flush();
	void update();

	template <ManagerOrSystem Type>
	void startup(std::optional<std::type_index> parentType = std::nullopt);
	template <ManagerOrSystem Type>
	void shutdown();
	template <ManagerOrSystem Type>
	Type* get();

	// storage access for registration
	friend class ManagerBase;
	friend class SystemBase;
};

#include "world.inl"