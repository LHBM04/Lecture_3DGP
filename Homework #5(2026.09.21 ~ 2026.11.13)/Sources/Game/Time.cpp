#include "Precompiled.hpp"
#include "Time.hpp"

#include "Framework/Platform.hpp"

namespace TUK::Game
{
	namespace
	{
		const TimeSubsystem* timeSubsystem = nullptr;
	}

	void Time::Initialize(TimeSubsystem* subsystem)
	{
		timeSubsystem = subsystem;
	}

	void Time::Release()
	{
		timeSubsystem = nullptr;
	}

	f32 Time::GetTimeScale()
	{
		return timeSubsystem->GetTimeScale();
	}

	f32 Time::GetDeltaTime()
	{
		return timeSubsystem->GetDeltaTime();
	}

	f32 Time::GetUnscaledDeltaTime()
	{
		return timeSubsystem->GetUnscaledDeltaTime();
	}

	f32 Time::GetFixedDeltaTime()
	{
		return timeSubsystem->GetFixedDeltaTime();
	}

	f32 Time::GetUnscaledFixedDeltaTime()
	{
		return timeSubsystem->GetUnscaledFixedDeltaTime();
	}
}
