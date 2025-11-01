#pragma once

template <typename ObjectType>
class NaiveAllocator final {
	std::vector<ObjectType*> objects;
	std::vector<uint16_t> refCounts; // TODO: implement this, also on pool and freelist.
public:
	explicit NaiveAllocator(size_t initialCapacity) {
		objects.reserve(initialCapacity);
		refCounts.reserve(initialCapacity);
	}
	~NaiveAllocator() { clear(); }

	Handle<ObjectType> create() {
		if (objects.size() >= std::numeric_limits<uint32_t>::max())
			throw std::runtime_error("Error: (Naive<" + std::string(typeid(ObjectType).name()) +">::allocate) Pool size limit reached.");
		auto id = static_cast<uint32_t>(objects.size());
		auto* object = new ObjectType(id);
		objects.push_back(object);
		return Handle<ObjectType>(id, 0);
	}
	NaiveAllocator& clone() {
		if constexpr (!std::is_copy_constructible_v<ObjectType>)
			throw std::runtime_error("Error: (Naive<" + std::string(typeid(ObjectType).name()) + ">::clone) ObjectType is not copy constructible.");
		auto newNaive = std::make_unique<NaiveAllocator>(objects.capacity());
		for (auto object : objects) {
			if (object) {
				auto* newObject = new ObjectType(*object);
				newNaive->objects.push_back(newObject);
				newNaive->refCounts.push_back(1);
			} else {
				newNaive->objects.push_back(nullptr);
				newNaive->refCounts.push_back(0);
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
	}
	void clear() {
		for (auto& object : objects)
			delete object;
		objects.clear();
		refCounts.clear();
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
		for (uint32_t i = 0; i < objects.size(); ++i)
			if (objects[i])
				handles.emplace_back(i, 0);
		return handles;
	}
	template <typename Function>
	void each(Function&& function) {
		for (auto* object : objects)
			if (object)
				function(object);
	}
};

