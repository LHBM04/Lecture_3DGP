#include "Precompiled.h"
#include "TimeContext.h"

#include "TimeSystem.h"

TimeContext::TimeContext(const TimeSystem* timeSystem_) noexcept
	: timeSystem{ timeSystem_ }
{
}

float TimeContext::GetTotalTime() const
{
	return timeSystem != nullptr ? timeSystem->GetTotalTime() : 0.0f;
}

float TimeContext::GetTimeScale() const
{
	return timeSystem != nullptr ? timeSystem->GetTimeScale() : 1.0f;
}

float TimeContext::GetDeltaTime() const
{
	return timeSystem != nullptr ? timeSystem->GetDeltaTime() : 0.0f;
}

float TimeContext::GetUnscaledDeltaTime() const
{
	return timeSystem != nullptr ? timeSystem->GetUnscaledDeltaTime() : 0.0f;
}

float TimeContext::GetFixedDeltaTime() const
{
	return timeSystem != nullptr ? timeSystem->GetFixedDeltaTime() : 0.0f;
}

float TimeContext::GetUnscaledFixedDeltaTime() const
{
	return timeSystem != nullptr ? timeSystem->GetUnscaledFixedDeltaTime() : 0.0f;
}

float TimeContext::GetFrameCount() const
{
	return timeSystem != nullptr ? timeSystem->GetFrameCount() : 0.0f;
}
