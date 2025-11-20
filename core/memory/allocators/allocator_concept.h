#pragma once

// resource(large) vs others(small)
// pool vs free list vs stack vs slab vs buddy vs garbage collection vs reference counting

class ComponentBase;
class Entity;

template <typename ConcreteAllocatorType, typename ObjectType>
concept AllocatorConcept =
	requires(ConcreteAllocatorType a, Handle<ObjectType> h) {
		{ a.clone() } -> std::same_as<ConcreteAllocatorType>;
		a.destroy(h);
		a.clear();
		{ a.valid(h) } -> std::same_as<bool>;
		{ a.resolve(h) } -> std::same_as<ObjectType*>;
		{ a.view() } -> std::same_as<std::vector<Handle<ObjectType>>>;
	} && (std::is_base_of_v<ComponentBase, ObjectType> ?
	requires(ConcreteAllocatorType a, Handle<Entity> e) {
		{ a.create(e) } -> std::same_as<Handle<ObjectType>>;
		a.each([](ObjectType*){});
		a.each([](Handle<Entity>, ObjectType*){});
	} :
	requires(ConcreteAllocatorType a) {
		{ a.create() } -> std::same_as<Handle<ObjectType>>;
	});
