#pragma once

#include "naive_allocator.h"

template <typename ObjectType> struct AllocatorSelector
	{ using type = NaiveAllocator<ObjectType>; }; // default allocator


template <> struct AllocatorSelector<class Scene>
	{ using type = NaiveAllocator<Scene>; };
template <> struct AllocatorSelector<class Entity>
	{ using type = NaiveAllocator<Entity>; };


// TODO: make API to set allocation policies for default/custom types
// TODO: different allocation policies depending on worlds
