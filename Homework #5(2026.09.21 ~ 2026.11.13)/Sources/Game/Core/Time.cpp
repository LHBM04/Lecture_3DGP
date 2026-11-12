#include "Precompiled.hpp"
#include "Time.hpp"

namespace TUK::Game
{
	namespace
	{
		const TimeSubsystem* timeSubsystem = nullptr;
	}

	void Time::Initialize(const TimeSubsystem* subsystem) noexcept
	{
		timeSubsystem = subsystem;
	}

	void Time::Release() noexcept
	{
		timeSubsystem = nullptr;
	}

	const TimeSubsystem* Time::GetSubsystem() noexcept
	{
		return timeSubsystem;
	}

	f32 Time::GetTimeScale() noexcept
	{
		return timeSubsystem->GetTimeScale();
	}

	f32 Time::GetDeltaTime() noexcept
	{
		return timeSubsystem->GetDeltaTime();
	}

	f32 Time::GetUnscaledDeltaTime() noexcept
	{
		return timeSubsystem->GetUnscaledDeltaTime();
	}

	f32 Time::GetFixedDeltaTime() noexcept
	{
		return timeSubsystem->GetFixedDeltaTime();
	}

	f32 Time::GetUnscaledFixedDeltaTime() noexcept
	{
		return timeSubsystem->GetUnscaledFixedDeltaTime();
	}
}
