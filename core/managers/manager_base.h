#ifndef GAME_ENGINE_MANAGER_BASE_H
#define GAME_ENGINE_MANAGER_BASE_H




namespace ikhanchoi {

class ManagerBase {
	class Context* context; // for accessing other managers via context
public:
	explicit ManagerBase(class Context* context) : context(context) {};
	virtual ~ManagerBase() = default;


	void setContext(Context* context) { this->context = context; }
	Context* getContext() { return context; };
};

}


#endif//GAME_ENGINE_MANAGER_BASE_H
