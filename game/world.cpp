#include "world.h"

#include "manager_base.h"


void World::update() {
	// collect events
	// handle events to collect commands
	// execute commands

	for (auto& [type, manager] : manager)
		manager->dispatch();
	for (auto& [type, manager] : manager)
		manager->flush();
}