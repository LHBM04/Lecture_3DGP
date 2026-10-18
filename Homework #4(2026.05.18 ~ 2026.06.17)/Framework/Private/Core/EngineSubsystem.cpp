#include "Precompiled.hpp"
#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	EngineSubsystem::EngineSubsystem(std::uint16_t priority) noexcept
		: Subsystem(priority)
	{
	}

	EngineSubsystem::~EngineSubsystem() noexcept
	{
	}

	SystemStatus EngineSubsystem::OnPreTick()
	{
		return SystemStatus::Continue;
	}

	SystemStatus EngineSubsystem::OnTick()
	{
		return SystemStatus::Continue;
	}

	SystemStatus EngineSubsystem::OnPostTick()
	{
		return SystemStatus::Continue;
	}

}