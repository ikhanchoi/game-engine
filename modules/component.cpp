#include "component.h"
#include "entity.h"


namespace ikhanchoi {
std::unique_ptr<ManagerBase> ComponentModule::generateManager(Context* context) {
	return std::make_unique<ComponentManager>(context);
}

}