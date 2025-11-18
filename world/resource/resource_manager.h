#pragma once
#include "world/manager_base.h"

class ResourceManager final : public ManagerBase {

	// ResourceBinder : this binds resources to components, this should know the member fields of each component.
public:
	explicit ResourceManager(World& world);
};