#ifndef GAME_ENGINE_POOL_H
#define GAME_ENGINE_POOL_H

#include "core/objects/object_base.h"

namespace ikhanchoi {

struct Handle {
    uint32_t index;
    uint32_t generation;
	std::type_index type; // object type
	Handle() : index(0), generation(0), type(typeid(void)) {}
	Handle(uint32_t index, uint32_t generation, std::type_index type) : index(index), generation(generation), type(type) {}
};

template <typename ObjectType>
struct Slot {
	alignas(ObjectType) char objectBuffer[sizeof(ObjectType)];
	uint32_t generation = 0;
	bool alive = false;
};

class PoolBase {
public:
	virtual ~PoolBase() = default;
	virtual Handle add(Object* untypedObject) = 0;
	virtual void remove(const Handle& handle) = 0;
	virtual Object* access(const Handle& handle) = 0;
	virtual void forEach(const std::function<void(Object*)>& function) = 0;
};

template <typename ObjectType>
class Pool : public PoolBase {
static_assert(std::is_base_of_v<Object, ObjectType> && !std::is_abstract_v<ObjectType>);
	std::vector<Slot<ObjectType>> slots;
    std::vector<uint32_t> frees;
public:
	explicit Pool(size_t initialCapacity = 64);
	Handle add(Object* untypedObject) override;
	void remove(const Handle& handle) override;
	Object* access(const Handle& handle) override; // It returns `Object*` instead of `ObjectType*` for overriding.
	void forEach(const std::function<void(Object*)>& function) override; // It is for up-casted object access.
	void forEach(const std::function<void(ObjectType*)>& function); // It is for down-casted object access.
};

#include "pool.inl"

}


#endif//GAME_ENGINE_POOL_H
