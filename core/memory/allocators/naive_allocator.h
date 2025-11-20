#pragma once

template <typename ObjectType>
class NaiveAllocator final {
	std::vector<ObjectType*> objects;
	std::vector<uint16_t> refCounts; // TODO: implement this, also on pool and freelist.

	std::conditional_t<std::is_base_of_v<class ComponentBase, ObjectType>, std::vector<Handle<Entity>>, std::nullptr_t> entityMap; // associated to objects

public:
	explicit NaiveAllocator(size_t initialCapacity) {
		objects.reserve(initialCapacity);
		refCounts.reserve(initialCapacity);
	}
	~NaiveAllocator() { clear(); }

	template <typename T = ObjectType>
	requires (!std::is_base_of_v<ComponentBase, T>)
	Handle<T> create() {
		if (objects.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (Naive<" + std::string(typeid(ObjectType).name()) + ">::create) Pool size limit reached.");
		auto id = static_cast<uint32_t>(objects.size());
		auto* object = new ObjectType();
		objects.push_back(object);
		return Handle<ObjectType>(id, 0);
	}

	template <typename T = ObjectType>
	requires (std::is_base_of_v<ComponentBase, T>)
	Handle<T> create(Handle<Entity> entity){
		if (objects.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (Naive<" + std::string(typeid(ObjectType).name()) + ">::create) Pool size limit reached.");
		auto id = static_cast<uint32_t>(objects.size());
		auto* object = new ObjectType();
		objects.push_back(object);
		entityMap.push_back(entity);
		return Handle<ObjectType>(id, 0);
	}

	NaiveAllocator clone() {
		if constexpr (!std::is_copy_constructible_v<ObjectType>)
			throw std::runtime_error("Error: (Naive<" + std::string(typeid(ObjectType).name()) + ">::clone) ObjectType is not copy constructible.");
		auto newNaive = std::make_unique<NaiveAllocator>(objects.capacity());
		for (size_t i = 0; i < objects.size(); ++i) {
			auto* object = objects[i];
			if (object) {
				auto* newObject = new ObjectType(*object);
				newNaive->objects.push_back(newObject);
				newNaive->refCounts.push_back(1);
				if constexpr (std::is_base_of_v<ComponentBase, ObjectType>)
					newNaive->entityMap.push_back(entityMap[i]);
			} else {
				newNaive->objects.push_back(nullptr);
				newNaive->refCounts.push_back(0);
				if constexpr (std::is_base_of_v<ComponentBase, ObjectType>)
					newNaive->entityMap.push_back(0);
			}
		}
		return newNaive;
	}

	void destroy(Handle<ObjectType> handle) {
		if (!valid(handle))
			throw std::runtime_error("Invalid or stale handle.");
		uint32_t id = handle.id;
		delete objects[id];
		objects[id] = nullptr;
		if constexpr (std::is_base_of_v<ComponentBase, ObjectType>)
			entityMap[id] = -1; // -1?
	}

	void clear() {
		for (auto& object : objects)
			delete object;
		objects.clear();
		refCounts.clear();
		if constexpr (std::is_base_of_v<ComponentBase, ObjectType>)
			entityMap.clear();
	}


	bool valid(Handle<ObjectType> handle) {
		if (handle.id >= objects.size())
			return false;
		if (handle.generation > 0)
			return false;
		if (objects[handle.id] == nullptr)
			return false;
		return true;

		return handle.id < objects.size() && handle.generation == 0 && objects[handle.id] != nullptr;
	}

	ObjectType* resolve(Handle<ObjectType> handle) {
		if (!valid(handle))
			throw std::runtime_error("Invalid or stale handle.");
		return objects[handle.id];
	}

	std::vector<Handle<ObjectType>> view() {
		std::vector<Handle<ObjectType>> handles;
		handles.reserve(objects.size());
		for (uint32_t i = 0; i < objects.size(); ++i)
			if (objects[i])
				handles.emplace_back(i, 0);
		return handles;
	}

	template <typename Function>
	void each(Function&& function) {
		for (size_t i = 0; i < objects.size(); ++i) {
			auto* object = objects[i];
			if (!object) continue;
			if constexpr (std::is_base_of_v<ComponentBase, ObjectType>) {
				if constexpr (std::is_invocable_v<Function, Handle<Entity>, ObjectType*>)
					function(entityMap[i], object);
				else if constexpr (std::is_invocable_v<Function, ObjectType*>)
					function(object);
				else
					static_assert(!std::is_same_v<Function, Function>, "Function signature not supported");
			} else {
				static_assert(std::is_invocable_v<Function, ObjectType*>, "Function must accept ObjectType*");
				function(object);
			}
		}
	}
};

