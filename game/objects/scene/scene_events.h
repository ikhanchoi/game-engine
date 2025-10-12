#pragma once
#include "core/execution/events/event_base.h"
#include "core/memory/handle.h"

class Scene;

struct MainSceneChangedEvent : EventBase {
	std::optional<Handle<Scene>> oldScene;
	std::optional<Handle<Scene>> newScene;

	template <typename OldScene, typename NewScene>
	MainSceneChangedEvent(OldScene&& oldScene, NewScene&& newScene)
		: oldScene(std::forward<OldScene>(oldScene)), newScene(std::forward<NewScene>(newScene)) {}
};