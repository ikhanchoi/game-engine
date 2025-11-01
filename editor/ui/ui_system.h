#pragma once
#include "world/system_base.h"

class UISystem final : public SystemBase {
	class SceneManager& sceneManager;

public:
	explicit UISystem(World& world);
	void tick() override;

	static void dockSpace();
	static void mainMenuBar();

	~UISystem() override;

};
