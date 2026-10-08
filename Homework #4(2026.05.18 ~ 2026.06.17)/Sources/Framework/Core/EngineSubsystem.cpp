#include "Precompiled.hpp"
#include "EngineSubsystem.hpp"

namespace TUK::Framework
{
	EngineSubsystem::EngineSubsystem(unsigned short priority) noexcept
		: Subsystem(priority)
	{
	}

	EngineSubsystem::~EngineSubsystem() noexcept
	{
	}

	void EngineSubsystem::OnPreTick()
	{
	}
	void EngineSubsystem::OnTick()
	{
	}
	void EngineSubsystem::OnPostTick()
	{
	}

}