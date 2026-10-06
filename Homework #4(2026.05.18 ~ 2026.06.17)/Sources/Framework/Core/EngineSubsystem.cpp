#include "Precompiled.h"
#include "EngineSubsystem.h"

namespace TUK::Framework
{
	EngineSubsystem::EngineSubsystem(unsigned short priority) noexcept
		: Subsystem(priority)
	{
	}

	EngineSubsystem::~EngineSubsystem() noexcept = default;

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