#ifndef GAME_ENGINE_COMPONENT_H
#define GAME_ENGINE_COMPONENT_H

#include <string>

namespace ikhanchoi {
class Component {
protected:
	int id;
	std::string name;
	std::string type;
};
}

#endif//GAME_ENGINE_COMPONENT_H
