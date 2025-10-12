#pragma once
#include <format>

template <ManagerOrSystem Type>
void World::startup() {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (manager.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::startup) Manager already exists for: {}", typeid(Type).name()));
		manager[typeid(Type)] = std::make_unique<Type>(*this);
	} else {
		if (system.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::startup) System already exists for: {}", typeid(Type).name()));
		system[typeid(Type)] = std::make_unique<Type>(*this);
	}
}

template <ManagerOrSystem Type>
void World::shutdown() {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (!manager.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::shutdown) Manager does not exist for: {}", typeid(Type).name()));
		manager[typeid(Type)].reset();
	} else {
		if (!system.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::shutdown) System does not exist for: {}", typeid(Type).name()));
		system[typeid(Type)].reset();
	}
}

template <ManagerOrSystem Type>
Type* World::get() {
	if constexpr (std::derived_from<Type, ManagerBase>) {
		if (!manager.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::get) Manager does not exist for: {}", typeid(Type).name()));
		return static_cast<Type*>(manager[typeid(Type)].get());
	} else {
		if (!system.contains(typeid(Type)))
			throw std::runtime_error(std::format("Error: (World::get) System does not exist for: {}", typeid(Type).name()));
		return static_cast<Type*>(system[typeid(Type)].get());
	}
}