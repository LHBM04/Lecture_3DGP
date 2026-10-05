#include "Precompiled.h"
#include "Subsystem.h"

namespace TUK::Framework
{
	Subsystem::Subsystem(unsigned short priority) noexcept
		: priority(priority)
	{
	}

	Subsystem::~Subsystem() noexcept
	{
	}

	unsigned short Subsystem::GetPriority() const noexcept
	{
		return priority;
	}

	void Subsystem::OnStartup()
	{
	}

	void Subsystem::OnPreTick()
	{
	}

	void Subsystem::OnTick()
	{
	}

	void Subsystem::OnPostTick()
	{
	}

	void Subsystem::OnShutdown()
	{
	}
}
