#include "Precompiled.hpp"
#include "Framework/Core/GameSubsystem.hpp"

namespace TUK::Framework
{
	GameSubsystem::GameSubsystem(unsigned short priority) noexcept
		: Subsystem(priority)
	{
	}

	GameSubsystem::~GameSubsystem() noexcept
	{
	}

	SystemStatus GameSubsystem::OnEarlyUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus GameSubsystem::OnFixedUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus GameSubsystem::OnUpdate()
	{
		return SystemStatus::Continue;
	}

	SystemStatus GameSubsystem::OnLateUpdate()
	{
		return SystemStatus::Continue;
	}
}
