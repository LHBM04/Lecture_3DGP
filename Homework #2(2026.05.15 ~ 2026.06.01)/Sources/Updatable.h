#pragma once

class TimeContext;

class IUpdatable
{
public:
	virtual ~IUpdatable() = default;

	virtual void OnUpdate(const TimeContext& context_) = 0;
	virtual void OnLateUpdate(const TimeContext& context_) = 0;
	virtual void OnFixedUpdate(const TimeContext& context_) = 0;
};