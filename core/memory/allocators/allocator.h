#pragma once

// resource(large) vs others(small)
// pool vs free list vs stack vs slab vs buddy vs garbage collection vs reference counting

template <typename ObjectType>
class Storage;

template <typename ObjectType>
class Allocator {
	friend class Storage<ObjectType>;
public:
	virtual ~Allocator() = default;

protected:
	virtual Handle<ObjectType> create() = 0;
	virtual std::unique_ptr<Allocator> clone() = 0;
	virtual void destroy(Handle<ObjectType>) = 0;
	virtual void clear() = 0;

	virtual bool valid(Handle<ObjectType>) = 0;
	virtual ObjectType* resolve(Handle<ObjectType>) = 0;
	virtual std::vector<Handle<ObjectType>> view() = 0;
	virtual void each(std::function<void(ObjectType*)>) = 0;
};

template <typename ObjectType, template <typename> class AllocatorType>
class AllocatorCRTP : public Allocator<ObjectType> {
protected:
	void each(std::function<void(ObjectType*)> function) override {
		static_cast<AllocatorType<ObjectType>*>(this)->_each(function);
	}
};
