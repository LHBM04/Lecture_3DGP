#pragma once

struct TimeContext;

class Updatable
{
public:
	virtual ~Updatable() = default;
	virtual void OnUpdate(const TimeContext& context_) = 0;
};
