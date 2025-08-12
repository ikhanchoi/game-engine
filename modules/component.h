#ifndef GAME_ENGINE_COMPONENT_H
#define GAME_ENGINE_COMPONENT_H

#include <string>
#include <typeindex>

#include "modules/core.h"
#include "modules/resource.h"

namespace ikhanchoi {

class ComponentModule : public CRTPModule<ComponentModule> {
public:
	static std::unique_ptr<ManagerBase> generateManager(Context* context);
};


class ComponentBase : public Base {

};

class RenderComponent : public ComponentBase, public CRTPObject<RenderComponent> {


};

class ComponentManager : public ManagerBase {
public:
	explicit ComponentManager(Context* context) : ManagerBase(context) {}


};

}

#endif//GAME_ENGINE_COMPONENT_H
