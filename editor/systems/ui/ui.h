#pragma once
#include "game/system_base.h"

class UISystem final : public SystemBase {

public:
	explicit UISystem(World& world);
	~UISystem() override;

};
