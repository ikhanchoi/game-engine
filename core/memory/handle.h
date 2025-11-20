#pragma once
#include <functional>

template <typename ObjectType>
struct Handle {
    uint32_t id;
    uint32_t generation;
	explicit Handle(uint32_t id, uint32_t generation) : id(id), generation(generation) {}
	bool operator == (const Handle& other) const { return id == other.id && generation == other.generation; }
	bool operator != (const Handle& other) const { return !(*this == other); }

	template <typename T=ObjectType>
	requires std::is_same_v<T, class Entity>
	uint32_t getId() const { return id; }
};

namespace std {
	template <typename ObjectType>
	struct hash<Handle<ObjectType>> {
		size_t operator()(const Handle<ObjectType>& h) const noexcept {
			uint64_t key = static_cast<uint64_t>(h.id) << 32 | h.generation;
			key ^= key >> 30;
			key *= 0xbf58476d1ce4e5b9ULL;
			key ^= key >> 27;
			key *= 0x94d049bb133111ebULL;
			key ^= key >> 31;
			if constexpr (sizeof(size_t) == 4)
				return static_cast<size_t>((key >> 32) ^ key);
			else
				return static_cast<size_t>(key);
		}
	};
}