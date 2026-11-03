#include "Precompiled.hpp"
#include "Time.hpp"

#include "Framework/Core.hpp"
#include "Framework/Platform.hpp"

#include "MyEngine.hpp"

namespace TUK::Game
{
	namespace
	{
		Framework::TimeSubsystem* time = nullptr;
	}

	float Time::GetDeltaTime()
	{
		if (!time) time = MyEngine::GetInstance()->GetSubsystem<Framework::TimeSubsystem>();
		return time->GetDeltaTime();
	}
}
