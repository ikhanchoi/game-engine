#pragma once
#include <unordered_map>
#include <typeindex>
#include <any>

template<typename Type>
concept ManagerOrSystem = std::is_base_of_v<class ManagerBase, Type> || std::is_base_of_v<class SystemBase, Type>;

class World {
	std::unordered_map<std::type_index, std::any> storage; // TODO: resolve external fragmentation
	std::unordered_map<std::type_index, std::unique_ptr<ManagerBase>> manager;
	std::unordered_map<std::type_index, std::unique_ptr<SystemBase>> system;

public:
	void update();

	template <ManagerOrSystem Type>
	void startup();
	template <ManagerOrSystem Type>
	void shutdown();
	template <ManagerOrSystem Type>
	Type* get();

	// storage access
	friend class ManagerBase;
	friend class SystemBase;
};

#include "world.inl"