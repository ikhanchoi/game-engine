#pragma once

struct EventBase {
	virtual ~EventBase() = 0;
};

inline EventBase::~EventBase() = default;