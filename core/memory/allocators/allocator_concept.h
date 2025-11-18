#pragma once

// resource(large) vs others(small)
// pool vs free list vs stack vs slab vs buddy vs garbage collection vs reference counting


template <typename ConcreteAllocatorType, typename ObjectType>
concept AllocatorConcept = requires(ConcreteAllocatorType a, Handle<ObjectType> h) {
	{ a.create() } -> std::same_as<Handle<ObjectType>>;
	{ a.clone() } -> std::same_as<ConcreteAllocatorType&>;
	{ a.destroy(h) };
	{ a.clear() };

	{ a.valid(h) } -> std::same_as<bool>;
	{ a.resolve(h) } -> std::same_as<ObjectType*>;
	{ a.view() } -> std::same_as<std::vector<Handle<ObjectType>>>;
	{ a.each([](ObjectType*){}) };
};
