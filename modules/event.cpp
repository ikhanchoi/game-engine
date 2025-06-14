#include "event.h"

namespace ikhanchoi {



/*-------------*/
/* EventSystem */
/*-------------*/


void EventSystem::setResourceManager(std::shared_ptr<ResourceManager> resourceManager) {
	this->resourceManager = resourceManager;
}

void EventSystem::setEntityManager(std::shared_ptr<EntityManager> entityManager) {
	this->entityManager = entityManager;
	/*
	subscribe<EntityUIEvent>([&entityManager](const EntityUIEvent& event) {
		if (event.type == "setName") {
			auto entity = entityManager->getEntity(event.entityId);
			if (entity) {
				entity->setName(event.name);
			}
		} else if (event.type == "removeEntity") {
			entityManager->removeEntity(event.entityId);
		} else if (event.type == "addEntity") {
			entityManager->addEntity(event.name);
		} else if (event.type == "setComponent") {
			auto entity = entityManager->getEntity(event.entityId);
			if (entity) {
				entity->setComponent(event.componentType, event.component);
			}
		}
	});*/
}

}