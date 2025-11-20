#include "world.h"
#include "manager_base.h"
#include "system_base.h"

void World::flush() {

	// lock_guard necessary
	systemExecutionGraph.traverse([this](const std::type_index& type) { system.at(type)->flush(); });

	for (auto& [type, manager] : manager)
		manager->flush();
}

void World::update() {
/*
	Time::Update();
	float deltaTime = Time::DeltaTime();
	accumulator += deltaTime;
	int fixedIterations = 0;
	while (accumulator >= fixedDeltaTime && fixedIterations < maxFixedIterations) {
		for (auto* sys : systems)
			sys->FixedTick(fixedDeltaTime);
		for (auto* sys : systems)
			sys->Dispatch();
		accumulator -= fixedDeltaTime;
		fixedIterations++;
	}
*/

	systemExecutionGraph.traverse([this](const std::type_index& type) { system.at(type)->tick(); });

	systemExecutionGraph.traverse([this](const std::type_index& type) { system.at(type)->dispatch(); });

	for (auto& [type, manager] : manager)
		manager->dispatch();

	flush();

}