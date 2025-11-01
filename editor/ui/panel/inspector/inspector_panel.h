#pragma once
#include "world/system_base.h"

class InspectorPanel : public SystemBase {

	std::optional<Handle<Entity>> selected;

public:
	explicit InspectorPanel(World& world) : SystemBase(world) {}
	void tick() override;

	void setSelected(std::optional<Handle<Entity>> entity);

	std::optional<Handle<Entity>> getSelected() const;

};