#pragma once

#include "naive_allocator.h"

template <typename ObjectType>
struct AllocatorSelector {
	using type = NaiveAllocator<ObjectType>;
};

class Scene;
class PersistentScene;
class Entity;

template <> struct AllocatorSelector<Scene> { using type = NaiveAllocator<Scene>; };
template <> struct AllocatorSelector<PersistentScene> { using type = NaiveAllocator<PersistentScene>; };
template <> struct AllocatorSelector<Entity> { using type = NaiveAllocator<Entity>; };

