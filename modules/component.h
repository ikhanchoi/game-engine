#ifndef GAME_ENGINE_COMPONENT_H
#define GAME_ENGINE_COMPONENT_H

#include <string>
#include <typeindex>

#include "modules/resource.h"

namespace ikhanchoi {


class ComponentBase {
public:
	virtual ~ComponentBase() = default;
};

class RenderComponent : public ComponentBase, public CRTPObject<RenderComponent> {


};

class ComponentManager : public PoolManagerBase {
public:
	explicit ComponentManager(Context* context) : PoolManagerBase(context) {}


};

}

#endif//GAME_ENGINE_COMPONENT_H
