#ifndef GAME_ENGINE_COMPONENT_H
#define GAME_ENGINE_COMPONENT_H

#include <string>
#include <typeindex>

#include "modules/core.h"
#include "modules/resource.h"

namespace ikhanchoi {

class Component : public Object {

public:
	virtual void visit(Visitor& visitor) = 0;

};

class RenderComponent : public Component {


};
}

#endif//GAME_ENGINE_COMPONENT_H
