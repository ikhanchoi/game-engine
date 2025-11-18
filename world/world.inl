#pragma once

template <ManagerOrSystem Type>
void World::startup(std::optional<std::type_index> parentType) {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (parentType.has_value())
			throw std::runtime_error("Error: (World::startup) Managers cannot have parent type: " + std::string(typeid(Type).name()));
		if (manager.contains(typeid(Type)))
			throw std::runtime_error("Error: (World::startup) Manager already exists for " + std::string(typeid(Type).name()));
		manager.emplace(typeid(Type), std::make_unique<Type>(*this));
	} else {
		if (!dependencyGraph.add(typeid(Type), parentType))
			throw std::runtime_error("Error: (World::startup) Parent Type does not exists for: " + std::string(typeid(Type).name()));
		if (system.contains(typeid(Type)))
			throw std::runtime_error("Error: (World::startup) System already exists for " + std::string(typeid(Type).name()));
		system.emplace(typeid(Type), std::make_unique<Type>(*this));
	}
}

template <ManagerOrSystem Type>
void World::shutdown() {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (!manager.contains(typeid(Type)))
			throw std::runtime_error("Error: (World::shutdown) Manager does not exist for " + std::string(typeid(Type).name()));
		manager[typeid(Type)].reset();
	} else {
		if (!dependencyGraph.remove(typeid(Type)))
			throw std::runtime_error("Error: (World::shutdown) Failed to remove from dependency graph for: " + std::string(typeid(Type).name()));
		if (!system.contains(typeid(Type)))
			throw std::runtime_error("Error: (World::shutdown) System does not exist for: " + std::string(typeid(Type).name()));
		system[typeid(Type)].reset();
	}
}

template <ManagerOrSystem Type>
Type* World::get() {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (!manager.contains(typeid(Type))) return nullptr;
		return static_cast<Type*>(manager[typeid(Type)].get());
	} else {
		if (!system.contains(typeid(Type))) return nullptr;
		return static_cast<Type*>(system[typeid(Type)].get());
	}
}