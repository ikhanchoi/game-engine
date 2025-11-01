#pragma once
#include "core/event/event_base.h"
#include "core/memory/handle.h"

class Scene;

struct CurrentSceneChangedEvent : EventBase {
	std::optional<Handle<Scene>> oldScene;
	std::optional<Handle<Scene>> newScene;

	template <typename OldScene, typename NewScene>
	CurrentSceneChangedEvent(OldScene&& oldScene, NewScene&& newScene)
		: oldScene(std::forward<OldScene>(oldScene)), newScene(std::forward<NewScene>(newScene)) {}
};