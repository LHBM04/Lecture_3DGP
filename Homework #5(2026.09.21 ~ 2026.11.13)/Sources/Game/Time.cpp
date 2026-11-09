#include "Precompiled.hpp"
#include "Time.hpp"

#include "Framework/Platform.hpp"

namespace TUK::Game
{
	namespace
	{
		// TimeSubsystem* timeSubsystem = nullptr;
	}

	void Time::Initialize()
	{
		// timeSubsystem = Engine::GetInstance().GetSubsystem<TimeSubsystem>();
	}

	void Time::Release()
	{
	}

	f32 Time::GetTimeScale()
	{
		return 1.0f;
	}

	f32 Time::GetDeltaTime()
	{
		return 1.0f * GetTimeScale();
	}

	f32 Time::GetUnscaledDeltaTime()
	{
		return 1.0f;
	}

	f32 Time::GetFixedDeltaTime()
	{
		return 1.0f * GetTimeScale();
	}

	f32 Time::GetUnscaledFixedDeltaTime()
	{
		return 1.0f;
	}
}
