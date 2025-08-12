#pragma once

template <typename ManagerType>
void Context::registerManager() {
	static_assert(std::is_base_of_v<ManagerBase, ManagerType>);
	if (manager.find(typeid(ManagerType)) != manager.end())
		throw std::runtime_error("Error: (Context::registerManager) Manager type already registered.");
	manager[typeid(ManagerType)] = std::move(std::make_unique<ManagerType>(this));
	if (!manager[typeid(ManagerType)])
		throw std::runtime_error("Error: (Context::registerManager) Failed to create manager.");
}

template <typename ManagerType>
ManagerType* Context::getManager() {
	static_assert(std::is_base_of_v<ManagerBase, ManagerType>);
	return dynamic_cast<ManagerType*>(getManager(typeid(ManagerType)));
}
