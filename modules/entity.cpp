#include "entity.h"

namespace ikhanchoi {


std::unique_ptr<ManagerBase> EntityModule::generateManager(Context* context) {
	return std::make_unique<EntityManager>(context);
}


Entity* EntityManager::addEntity(const std::string &name, ikhanchoi::Entity *parent) {
	uint32_t id = globalEntityId++;
	auto& siblings = parent ? parent->getChildren() : entities;
	siblings.emplace_back(std::make_unique<Entity>(id, name))->setParent(parent);
	return siblings.back().get();
}

void EntityManager::removeEntity(Entity *entity) {
	auto& siblings = entity->getParent() ? entity->getParent()->getChildren() : entities;
	siblings.erase(std::remove_if(siblings.begin(), siblings.end(),
		[&](const std::unique_ptr<Entity>& e) { return e.get() == entity; }), siblings.end());
}

void EntityManager::moveEntity(Entity *entity, Entity *parent) { // move entity to a new parent
	auto& currentSiblings = entity->getParent() ? entity->getParent()->getChildren() : entities;
	std::unique_ptr<Entity> temporaryEntity;
	auto it = std::remove_if(currentSiblings.begin(), currentSiblings.end(),
		[&](const std::unique_ptr<Entity>& e) {
			if (e.get() == entity) {
				temporaryEntity = std::move(const_cast<std::unique_ptr<Entity>&>(e)); // Extract the unique_ptr
				return true; // Mark for removal from current vector
			}
			return false;
		});
	currentSiblings.erase(it, currentSiblings.end());
	if (!temporaryEntity)
		throw std::runtime_error("Error: (EntityManager::moveEntity) Entity not found.");

	temporaryEntity->setParent(parent);
	auto& siblings = parent ? parent->getChildren() : entities;
	siblings.push_back(std::move(temporaryEntity));
}


}